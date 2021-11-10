#include <stddef.h>
#include "vm.h"
#include "process.h"
#include "entity.h"
#include "ancestor.h"
#include "MD5.h"
#include <M5StickC.h>
#include "config.h"

VM::VM(long seed) {
    process.startPoint = 0;
    soupSize = SOUP_SIZE;
    for (int i = 0; i < SOUP_SIZE; i++) {
        soup[i] = 0;
    }
    nextPosition = 0;
    entities = GetEntity(sizeof(ancestorData));
    if (entities == NULL) {
        crashed = true;
        return;
    }
    crashed = false;
    totalExecutedInstructions = 0;
    CopyCreature(ancestorData, soup + entities->startPoint, sizeof(ancestorData));
    CreateID(seed);
    Serial.print("ID: ");
    Serial.print(id);
    Serial.print("\n");
}

VM::~VM() {
    CleanEntities();
    free(id);
}

bool VM::isCrashed() {
    return crashed;
}

int VM::GetStatus(Status statusList[], int max) {
    Entity *currentEntity = entities;
    int index = 0;
    while (currentEntity != NULL) {
        statusList[index].index = index + 1;
        statusList[index].length = entities->size;
        statusList[index].life_time = 0;

        strncpy(statusList[index].hash, entities->hash, 32);
        statusList[index].hash[32] = 0;

        currentEntity = entities->next;
        index++;
        if (max >= index) {
            currentEntity = NULL;
            break;
        }
    }

    return index;
}

void VM::CreateID(long seed) {
    char buf[60];
    sprintf(buf, "creature%d", seed);
    unsigned char *hash = MD5::make_hash(buf, 60);
    id = MD5::make_digest(hash, 16);
    free(hash);
}

void VM::CleanEntities() {
    Entity *entity = entities;
    while (entity != NULL) {
        Entity *nextEntity = entity->next;
        delete entity;
        entity = nextEntity;
    }
    free(id);
}

void VM::CopyCreature(char *source, char *destination, int length) {
    for (int i; i < length; i++) {
        *destination++ = *source++;
    }
}

Entity *VM::GetEntity(int size) {
    int allocatedPosition = AllocateMemory(size);
    if (allocatedPosition == -1) {
        return NULL;
    }
    Entity *entity = new Entity(allocatedPosition, size, soup + allocatedPosition, 0);
    return entity;
}

int VM::FindEmptySpace(int size, bool debug) {
    int searchPosition = 0;
    while (searchPosition + size < soupSize) {
        if (soup[searchPosition] == 0) {
            if (debug) {
                Serial.print("Target\n");
                Serial.print("\n");
            }
            int targetPosition = searchPosition;
            bool notEnough = false;
            for (int i = 0; i < size; i++) {
                if (soup[searchPosition] != 0) {
                    if (debug) {
                        Serial.print("Not Target\n");
                        Serial.print(searchPosition);
                        Serial.print("\n");
                    }
                    searchPosition++;
                    notEnough = true;
                    break;
                }
                searchPosition++;
                if (debug) {
                    Serial.print(searchPosition);
                    Serial.print("\n");
                }
            }
            if (!notEnough) {
                Serial.print("Found Position: ");
                Serial.print(targetPosition);
                Serial.print("\n\n");
                return targetPosition;
            }
        } else {
            searchPosition++;
            if (debug) {
                Serial.print(searchPosition);
                Serial.print("\n");
            }
        }
    }
    return -1;
}

void VM::DeleteEntity(Entity *entity) {
    int currentPosition = entity->startPoint;
    for (int i = 0; i < entity->size; i++) {
        soup[currentPosition] = 0;
        currentPosition++;
    }
    Serial.print("Clean up completed\n");
    delete entity;
    Serial.print("Clean up completed2\n");
}

int VM::AllocateMemory(int size) {
    int position = FindEmptySpace(size, false);
    if (position >= 0) {
        return position;
    }

    // purge
    Serial.print("Purging... \n");
    int count = 0;
    Entity *entity = entities;
    Entity *previousEntity = NULL;
    while (entity != NULL) {
        long randNumber = random(1, 10);
        Entity *nextEntity = entity->next;
        if (entity->startPoint == process.startPoint) {
            Serial.print("Current Process\n");
        } else if (entity->startPoint == process.previousStartPoint ) {
            Serial.print("Previous Process\n");
        } else if (entity->startPoint == process.nextStartPoint ) {
            Serial.print("Next Process\n");
        } else if ( randNumber < PURGE_RATIO) {
            count++;
            DeleteEntity(entity);
            if (previousEntity == NULL) {
                entities = nextEntity;
            } else {
                previousEntity->next = nextEntity;
            }
        } else {
            previousEntity = entity;
        }
        entity = nextEntity;
    }
    Serial.print(count);
    Serial.print(" entities purged.\n\n");
    return FindEmptySpace(size, false);

}

void VM::IntroduceMutation(Entity *entry) {
    long randNumber = random(entry->size);
    int bit = (int) random(4);
    Serial.print("mutation ");
    int index = (int) randNumber + entry->startPoint;
    char data = soup[index];
    Serial.print((int) data, HEX);
    soup[index] = data ^ ((1 << bit) & 0x1f);
    Serial.print(" -> ");
    Serial.print((int) soup[index], HEX);
    Serial.print("\n");
}

void VM::OneLifeCycle() {
    if (crashed) {
        Serial.print("VM has been crashed\n");
        return;
    } else {
        Serial.print("Proceed\n");
    }
    Entity *entity = entities;
    Entity *previousEntity = NULL;

    unsigned long time = millis();

    while (entity != NULL) {
        Entity *nextEntity = entity->next;
        if (entity->startTime < time) {
            Serial.print("Start New Entity");
            int error = Execute(entity, time, previousEntity, nextEntity);
            if (error == COMPLETE) {
                previousEntity = entity;
                Serial.print("Process has been completed\n");
                if (totalExecutedInstructions > MUTATION_RATIO) {
                    IntroduceMutation(entity);
                    totalExecutedInstructions = 0;
                }
            } else {
                Serial.print("Process has been crashed\n");
                DeleteEntity(entity);
                Serial.print("Entity deleted\n");
                if (previousEntity == NULL) {
                    entities = nextEntity;
                } else {
                    previousEntity->next = nextEntity;
                }
            }
        }
        entity = nextEntity;
    }
}

int VM::Execute(Entity *entry, unsigned long startTime, Entity *previousEntry, Entity *nextEntry) {
    process.ax = 0;
    process.bx = 0;
    process.cx = 0;
    process.dx = 0;
    process.startPoint = entry->startPoint;
    process.size = entry->size;
    process.ip = process.startPoint;
    process.fl = 0;
    process.sp = 0;
    process.startTime = startTime;
    process.error = NO_ERROR;
    int position = process.startPoint;
    int process_step_count = 0;
    if (previousEntry != NULL) {
        process.previousStartPoint = previousEntry->startPoint;
    } else {
        process.previousStartPoint = -1;
    }
    if (nextEntry != NULL) {
        process.nextStartPoint = nextEntry->startPoint;
    } else {
        process.nextStartPoint = -1;
    }
    while (process.error == NO_ERROR) {

        int currentPosition = position;
        int currentCommand = (int) (soup[position]);

        /*
        Serial.print("Debug: ----------\n");
        Serial.print("AX: ");
        Serial.print(process.ax);
        Serial.print(" BX: ");
        Serial.print(process.bx);
        Serial.print(" CX: ");
        Serial.print(process.cx);
        Serial.print(" DX: ");
        Serial.print(process.dx);
        Serial.print(" Step:");
        Serial.print(currentPosition);

        Serial.print(" Command:");
        Serial.print(currentCommand, HEX);
        Serial.print("\n");
        */

        position = ExecuteCommand(position);
        totalExecutedInstructions++;
        if (position == currentPosition && process.error == NO_ERROR) {
            int command = (int) (soup[position]);
            Serial.print(command, HEX);
            Serial.print("<= Command not proceed\n");
            process.error = NOT_PROCEED;
        }

        process_step_count++;
        if (process_step_count > 5000) {
            process.error = TOO_MANY_STEP;
            Serial.print("Too Many Step\n");
        }

    }

    if (process.error != COMPLETE) {
        Serial.print("Error:");
        Serial.print(process.error);
        Serial.print("\nPosition: ");
        Serial.print(position);
        Serial.print("\nCommand: ");
        Serial.print((int) soup[position], HEX);
        if (position > 0) {
            Serial.print("\nPrev Command: ");
            Serial.print((int) soup[position - 1], HEX);
        }
    }

    return process.error;
}

void VM::Push(int value) {
    if (process.sp >= 10) {
        process.error = STACK_OVER_FLOW;
        return;
    }
    process.stack[process.sp++] = value;
    return;
}

int VM::Pop() {
    if (process.sp <= 0) {
        process.error = STACK_UNDER_FLOW;
        return 0;
    }
    int val = process.stack[--process.sp];
    process.stack[process.sp] = 0;
    return val;
}

bool VM::MatchPattern(char *pattern, int position) {
    return (pattern[0] == soup[position] &&
            pattern[1] == soup[position + 1] &&
            pattern[2] == soup[position + 2] &&
            pattern[3] == soup[position + 3]);
}

int VM::FindTemplate(char *originalPattern, int position, directionType direction) {
    int currentPosition = 0;
    int foundPosition = -1;
    int foundCount = -1;
    int count = 0;

    char pattern[4];
    pattern[0] = originalPattern[0] ^ 1;
    pattern[1] = originalPattern[1] ^ 1;
    pattern[2] = originalPattern[2] ^ 1;
    pattern[3] = originalPattern[3] ^ 1;

    /*
    Serial.print("\nFind: ");
    Serial.print((int)pattern[0]);
    Serial.print((int)pattern[1]);
    Serial.print((int)pattern[2]);
    Serial.print((int)pattern[3]);
    Serial.print("\n");
    */

    if (direction == BACKWARD || direction == NEAREST) {
        currentPosition = position;
        while (1) {
            if (soup[currentPosition] != 0x00 && soup[currentPosition] != 0x01) {
                currentPosition -= 4;
                count += 4;
            } else {
                if (MatchPattern(pattern, currentPosition)) {
                    foundPosition = currentPosition;
                    foundCount = count;
                    break;
                }
                currentPosition--;
                count++;
            }
            if (currentPosition < 0 || count > TEMPLATE_SEARCH_RANGE) {
                break;
            }
        }
    }
    if (direction == FORWARD || direction == NEAREST) {
        currentPosition = position;
        while (1) {
            if (soup[currentPosition] != 0x00 && soup[currentPosition] != 0x01) {
                currentPosition++;
                count++;
            } else {
                if (MatchPattern(pattern, currentPosition)) {
                    if (direction == NEAREST && foundCount >= 0) {
                        if (count < foundCount) {
                            foundPosition = currentPosition;
                            foundCount = count;
                        }
                    } else {
                        foundPosition = currentPosition;
                        foundCount = count;
                    }
                    break;
                }
                currentPosition++;
                count++;
            }
            if (currentPosition >= soupSize || count > TEMPLATE_SEARCH_RANGE) {
                break;
            }
        }
    }

    if (foundPosition >= 0) {
        foundPosition += 4;
    }

    /*
    Serial.print("Result: ");
    Serial.print(foundPosition);
    Serial.print("\n");
    */

    return foundPosition;
}

int VM::Command_Nop(int position) {
    return ++position;
}

int VM::Command_Or1(int position) {
    process.cx ^= 1;
    return ++position;
}

int VM::Command_Shl(int position) {
    process.cx <<= 1;
    return ++position;
}

int VM::Command_Zero(int position) {
    process.cx = 0;
    return ++position;
}

int VM::Command_If_Cz(int position) {
    if (process.cx != 0) {
        position++;
    }
    return ++position;
}

int VM::Command_Sub_Ab(int position) {
    process.cx = process.ax - process.bx;
    return ++position;
}

int VM::Command_Sub_Ac(int position) {
    process.ax = process.ax - process.cx;
    return ++position;
}

int VM::Command_Inc_A(int position) {
    process.ax++;
    return ++position;
}

int VM::Command_Inc_B(int position) {
    process.bx++;
    return ++position;
}

int VM::Command_Dec_C(int position) {
    process.cx--;
    return ++position;
}

int VM::Command_Inc_C(int position) {
    process.cx++;
    return ++position;
}

int VM::Command_Push_Ax(int position) {
    Push(process.ax);
    return ++position;
}

int VM::Command_Push_Bx(int position) {
    Push(process.bx);
    return ++position;
}

int VM::Command_Push_Cx(int position) {
    Push(process.cx);
    return ++position;
}

int VM::Command_Push_Dx(int position) {
    Push(process.dx);
    return ++position;
}

int VM::Command_Pop_Ax(int position) {
    process.ax = Pop();
    return ++position;
}

int VM::Command_Pop_Bx(int position) {
    process.bx = Pop();
    return ++position;
}

int VM::Command_Pop_Cx(int position) {
    process.cx = Pop();
    return ++position;
}

int VM::Command_Pop_Dx(int position) {
    process.dx = Pop();
    return ++position;
}

int VM::Command_Jmp(int position) {
    char pattern[4];
    if (position + 4 >= soupSize) {
        process.error = EXECUTE_OVERFLOW;
        return position;
    }
    pattern[0] = soup[position + 1];
    pattern[1] = soup[position + 2];
    pattern[2] = soup[position + 3];
    pattern[3] = soup[position + 4];
    int foundPosition = FindTemplate(pattern, position, NEAREST);
    if (foundPosition == -1) {
        process.error = JUMP_TARGET_NOT_FOUND;
        return position;
    }

    position = foundPosition;
    return position;
}

int VM::Command_JmpB(int position) {
    char pattern[4];
    if (position + 4 >= soupSize) {
        process.error = EXECUTE_OVERFLOW;
        return position;
    }
    pattern[0] = soup[position + 1];
    pattern[1] = soup[position + 2];
    pattern[2] = soup[position + 3];
    pattern[3] = soup[position + 4];
    int foundPosition = FindTemplate(pattern, position, BACKWARD);
    if (foundPosition == -1) {
        process.error = JUMP_TARGET_NOT_FOUND;
        return position;
    }

    position = foundPosition;
    return position;
}

int VM::Command_Call(int position) {
    char pattern[4];
    if (position + 4 >= soupSize) {
        process.error = EXECUTE_OVERFLOW;
        return position;
    }
    pattern[0] = soup[position + 1];
    pattern[1] = soup[position + 2];
    pattern[2] = soup[position + 3];
    pattern[3] = soup[position + 4];
    int foundPosition = FindTemplate(pattern, position, NEAREST);
    if (foundPosition == -1) {
        process.error = JUMP_TARGET_NOT_FOUND;
        return position;
    }
    Push(position + 1);
    position = foundPosition;
    return position;
}

int VM::Command_Ret(int position) {
    position = Pop();
    return position;
}

int VM::Command_Mov_CD(int position) {
    process.dx = process.cx;
    return ++position;
}

// move ax to bx, bx=ax
int VM::Command_Mov_AB(int position) {
    process.bx = process.ax;
    return ++position;
}

// move instruction at [bx] to [ax], [ax]=[bx]
int VM::Command_Mov_IAB(int position) {

    if (process.ax < process.daughterStartPoint || process.ax >= process.daughterStartPoint + process.daughterSize) {
        process.error = COPY_OVER_FLOW;
        return position;
    }
    if (process.bx < process.startPoint ||
        process.bx >= process.startPoint + process.size) {
        process.error = COPY_OVER_FLOW;
        return position;
    }
    if (process.bx < 0 || process.ax < 0 || process.ax >= soupSize || process.bx >= soupSize) {
        process.error = COPY_OVER_FLOW;
        return position;
    }
    soup[process.ax] = soup[process.bx];
    return ++position;
}

int VM::Command_Adr_(directionType direction, int position) {
    char pattern[4];
    if (position + 4 >= soupSize) {
        process.error = EXECUTE_OVERFLOW;
        return position;
    }
    pattern[0] = soup[position + 1];
    pattern[1] = soup[position + 2];
    pattern[2] = soup[position + 3];
    pattern[3] = soup[position + 4];

    process.ax = FindTemplate(pattern, position, direction);

    return ++position;
}

// 	search template (nearest) address to ax
int VM::Command_Adr(int position) {
    return Command_Adr_(NEAREST, position);
}

// search template (backward) address to ax
int VM::Command_AdrB(int position) {
    return Command_Adr_(BACKWARD, position);
}

// search template (forward) address to ax
int VM::Command_AdrF(int position) {
    return Command_Adr_(FORWARD, position);
}

// 	allocate memory, cx=size, return address in ax
int VM::Command_Mal(int position) {
    process.ax = AllocateMemory(process.cx);
    if (process.ax == -1) {
        process.error = CAN_NOT_MEMORY_ALLOCATION;
        return ++position;
    }
    Serial.print("Allocate daughter memory");
    Serial.print("Position:");
    Serial.print(process.ax);
    Serial.print("\nsize:");
    Serial.print(process.cx);
    Serial.print("\n");

    process.daughterSize = process.cx;
    process.daughterStartPoint = process.ax;
    return ++position;
}

int VM::Command_Divide(int position) {
    // Create new entity
    Entity *entity = new Entity(process.daughterStartPoint, process.daughterSize, soup + process.daughterStartPoint,
                                process.startTime);
    Entity *currentEntity = entities;
    int index = 1;
    while (currentEntity->next != NULL) {
        currentEntity = currentEntity->next;
        index++;
    }
    currentEntity->next = entity;
    Serial.print("=================================> New Entity: Index:");
    Serial.print(index + 1);
    Serial.print("  Hash:");
    Serial.print(entity->hash);
    Serial.print("\n");

    process.error = COMPLETE;
    return ++position;
}

int VM::ExecuteCommand(int position) {
    switch (soup[position]) {
        case 0x00: // nop_0
        case 0x01: // nop_1
            return Command_Nop(position);
        case 0x02: // or1
            return Command_Or1(position);
        case 0x03: // shl
            return Command_Shl(position);
        case 0x04: // zero
            return Command_Shl(position);
        case 0x05: // if_cz
            return Command_If_Cz(position);
        case 0x06: // sub_ab
            return Command_Sub_Ab(position);
        case 0x07: // sub_ab
            return Command_Sub_Ac(position);
        case 0x08:
            return Command_Inc_A(position);
        case 0x09:
            return Command_Inc_B(position);
        case 0x0a:
            return Command_Dec_C(position);
        case 0x0b:
            return Command_Inc_C(position);
        case 0x0c:
            return Command_Push_Ax(position);
        case 0x0d:
            return Command_Push_Bx(position);
        case 0x0e:
            return Command_Push_Cx(position);
        case 0x0f:
            return Command_Push_Dx(position);
        case 0x10:
            return Command_Pop_Ax(position);
        case 0x11:
            return Command_Pop_Bx(position);
        case 0x12:
            return Command_Pop_Cx(position);
        case 0x13:
            return Command_Pop_Dx(position);
        case 0x14:
            return Command_Jmp(position);
        case 0x15:
            return Command_JmpB(position);
        case 0x16:
            return Command_Call(position);
        case 0x17:
            return Command_Ret(position);
        case 0x18:
            return Command_Mov_CD(position);
        case 0x19:
            return Command_Mov_AB(position);
        case 0x1a:
            return Command_Mov_IAB(position);
        case 0x1b:
            return Command_Adr(position);
        case 0x1c:
            return Command_AdrB(position);
        case 0x1d:
            return Command_AdrF(position);
        case 0x1e:
            return Command_Mal(position);
        case 0x1f:
            return Command_Divide(position);
    }
    return Command_Nop(position);
}
