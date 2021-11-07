#include <stddef.h>
#include "vm.h"
#include "process.h"
#include "entity.h"
#include "ancestor.h"
#include "MD5.h"

VM::VM(long seed) {
    for (int i = 0; i < SOUP_SIZE; i++) {
        soup[i] = 0;
    }
    nextPosition = 0;
    entities = GetEntity(sizeof(ancestorData));
    CopyCreature(ancestorData, soup + entities->startPoint, sizeof(ancestorData));
    CreateID(seed);
}

VM::~VM() {
    CleanEntities();
}

int VM::GetStatus(Status statusList[], int max) {
    Entity *currentEntity = entities;
    int index = 0;
    while( currentEntity != NULL ){
        statusList[index].index = index+1;
        statusList[index].length = entities->size;
        statusList[index].life_time = 0;

        strncpy(statusList[index].hash, "161b6bc86389b8b1fe6e8390e9618c9d", 32);
        statusList[index].hash[32] = 0;

        currentEntity = entities->next;
        index ++;
        if( max >= index){
            currentEntity = NULL;
            break;
        }
    }

    return index;
}

void VM::CreateID(long seed) {
    char buf[60];
    sprintf(buf, "creature%d", seed );
    MD5 generator = MD5();
    unsigned char *hash = generator.make_hash(buf, 60);
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
    Entity *entity = new Entity(allocatedPosition, size);
    return entity;
}

int VM::AllocateMemory(int size) {
    if (nextPosition + size <= soupSize) {
        int allocatedPosition = nextPosition;
        nextPosition += size;
        return allocatedPosition;
    }
    // [TODO] Purge

    return 0;
}

void VM::OneLifeCycle() {
    Entity *entity = entities;
    Entity *previousEntity = NULL;
    while (entity != NULL) {
        Entity *nextEntity = entity->next;
        int error = Execute(entity);
        if( error == COMPLETE ){
            previousEntity = entity;
        }else{
            delete entity;
            if( previousEntity == NULL ){
                entities = nextEntity;
            }else{
                previousEntity->next = nextEntity;
            }
        }
        entity = nextEntity;
    }
}

int VM::Execute(Entity *entry) {
    process.ax = 0;
    process.bx = 0;
    process.cx = 0;
    process.dx = 0;
    process.startPoint = entry->startPoint;
    process.size = entry->size;
    process.ip = process.startPoint;
    process.fl = 0;
    process.sp = 0;
    process.error = NO_ERROR;
    int position = process.startPoint;
    while(process.error == NO_ERROR) {
        position = ExecuteCommand(position);
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

int VM::FindTemplate(char *pattern, int position, directionType direction) {
    int currentPosition = 0;
    int foundPosition = -1;
    int foundCount = -1;
    int count = 0;
    if (direction == BACKWARD || direction == NEAREST) {
        currentPosition = position;
        while (1) {
            if (soup[position] != 0x00 && soup[position] != 0x01) {
                currentPosition -= 4;
                count += 4;
            } else {
                if (MatchPattern(pattern, position)) {
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
            if (soup[position] != 0x00 && soup[position] != 0x01) {
                currentPosition++;
                count++;
            } else {
                if (MatchPattern(pattern, position)) {
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
        return 0;
    }
    pattern[0] = soup[position + 1];
    pattern[1] = soup[position + 2];
    pattern[2] = soup[position + 3];
    pattern[3] = soup[position + 4];
    int foundPosition = FindTemplate(pattern, position, NEAREST);
    if (foundPosition == -1) {
        process.error = JUMP_TARGET_NOT_FOUND;
        return 0;
    }

    position = foundPosition;
    return position;
}

int VM::Command_JmpB(int position) {
    char pattern[4];
    if (position + 4 >= soupSize) {
        process.error = EXECUTE_OVERFLOW;
        return 0;
    }
    pattern[0] = soup[position + 1];
    pattern[1] = soup[position + 2];
    pattern[2] = soup[position + 3];
    pattern[3] = soup[position + 4];
    int foundPosition = FindTemplate(pattern, position, BACKWARD);
    if (foundPosition == -1) {
        process.error = JUMP_TARGET_NOT_FOUND;
        return 0;
    }

    position = foundPosition;
    return position;
}

int VM::Command_Call(int position) {
    char pattern[4];
    if (position + 4 >= soupSize) {
        process.error = EXECUTE_OVERFLOW;
        return 0;
    }
    pattern[0] = soup[position + 1];
    pattern[1] = soup[position + 2];
    pattern[2] = soup[position + 3];
    pattern[3] = soup[position + 4];
    int foundPosition = FindTemplate(pattern, position, NEAREST);
    if (foundPosition == -1) {
        process.error = JUMP_TARGET_NOT_FOUND;
        return 0;
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
    if (process.ax < process.startPoint || process.ax >= process.startPoint + process.size) {
        process.error = COPY_OVER_FLOW;
        return 0;
    }
    if (process.bx < process.daughterStartPoint ||
        process.bx >= process.daughterStartPoint + process.daughterSize) {
        process.error = COPY_OVER_FLOW;
        return 0;
    }
    if (process.bx < 0 || process.ax < 0 || process.ax >= soupSize || process.bx >= soupSize) {
        process.error = COPY_OVER_FLOW;
        return 0;
    }
    soup[process.bx] = soup[process.ax];
    return ++position;
}

int VM::Command_Adr_(directionType direction, int position) {
    char pattern[4];
    if (position + 4 >= soupSize) {
        process.error = EXECUTE_OVERFLOW;
        return 0;
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
    process.daughterSize = process.cx;
    process.daughterStartPoint = process.ax;
    return ++position;
}

int VM::Command_Divide(int position) {

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
