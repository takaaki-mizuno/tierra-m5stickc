#include "vm.h"
#include "process.h"
#include "entity.h"

VM::VM(int size) {
    soupSize = size;
    soup = new char[size];
    for (int i = 0; i < size; i++) {
        soup[i] = 0;
    }
    nextPosition = 0;
}

VM::~VM() {
    if (soup != null) {
        delete[] soup;
    }
}

void VM::introduceFragment(char *fragment, int length) {

}

void VM::allocateMemory(int size, Process *process) {
    if( nextPosition + size <= soupSize ){
        
    }
}

void VM::Execute(Entity *entry) {
    Process process;
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
}

void VM::Push(int value, Process *process) {
    if (process->sp >= 10) {
        process->error = STACK_OVER_FLOW;
        return;
    }
    process->stack[process->sp++] = value;
    return;
}

int VM::Pop(Process *process) {
    if (process->sp <= 0 >) {
        process->error = STACK_UNDER_FLOW;
        return;
    }
    int val = process->stack[--process->sp];
    process->stack[process->sp] = 0;
    return val;
}

bool VM::MatchPattern(char *pattern, int position, Process *process) {
    return (pattern[0] == soup[position] &&
            pattern[1] == soup[position + 1] &&
            pattern[2] == soup[position + 2] &&
            pattern[3] == soup[position + 3])
}

int VM::FindTemplate(char *pattern, int position, directionType direction, Process *process) {
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
                if (MatchPattern(pattern, position, process)) {
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
                if (MatchPattern(pattern, position, process)) {
                    if (direction == NEAREST && foundCount >= 0) {
                        if ( && count < foundCount ){
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

int VM::Command_Nop(int position, Process *process) {
    return ++position;
}

int VM::Command_Or1(int position, Process *process) {
    process->cx ^= 1;
    return ++position;
}

int VM::Command_Shl(int position, Process *process) {
    process->cx <<= 1;
    return ++position;
}

int VM::Command_Zero(int position, Process *process) {
    process->cx = 0;
    return ++position;
}

int VM::Command_If_Cz(int position, Process *process) {
    if (process->cx != 0) {
        position++;
    }
    return ++position;
}

int VM::Command_Sub_Ab(int position, Process *process) {
    process->cx = process->ax - process->bx;
    return ++position;
}

int VM::Command_Sub_Ac(int position, Process *process) {
    process->ax = process->ax - process->cx;
    return ++position;
}

int VM::Command_Inc_A(int position, Process *process) {
    process->ax++;
    return ++position;
}

int VM::Command_Inc_B(int position, Process *process) {
    process->bx++;
    return ++position;
}

int VM::Command_Dec_C(int position, Process *process) {
    process->cx--;
    return ++position;
}

int VM::Command_Inc_C(int position, Process *process) {
    process->cx++;
    return ++position;
}

int VM::Command_Push_Ax(int position, Process *process) {
    Push(process->ax, process);
    return ++position;
}

int VM::Command_Push_Bx(int position, Process *process) {
    Push(process->bx, process);
    return ++position;
}

int VM::Command_Push_Cx(int position, Process *process) {
    Push(process->cx, process);
    return ++position;
}

int VM::Command_Push_Dx(int position, Process *process) {
    Push(process->dx, process);
    return ++position;
}

int VM::Command_Pop_Ax(int position, Process *process) {
    process->ax = Pop(process);
    return ++position;
}

int VM::Command_Pop_Bx(int position, Process *process) {
    process->bx = Pop(process);
    return ++position;
}

int VM::Command_Pop_Cx(int position, Process *process) {
    process->cx = Pop(process);
    return ++position;
}

int VM::Command_Pop_Dx(int position, Process *process) {
    process->dx = Pop(process);
    return ++position;
}

int VM::Command_Jmp(int position, Process *process) {
    char pattern[4];
    if (position + 4 >= soupSize) {
        process->error = EXECUTE_OVERFLOW;
        return;
    }
    pattern[0] = soup[position + 1];
    pattern[1] = soup[position + 2];
    pattern[2] = soup[position + 3];
    pattern[3] = soup[position + 4];
    foundPosition = FindTemplate(pattern, position, NEAREST, process);
    if (foundPosition == -1) {
        process->error = JUMP_TARGET_NOT_FOUND;
        return;
    }

    position = foundPosition;
    return position;
}

int VM::Command_JmpB(int position, Process *process) {
    char pattern[4];
    if (position + 4 >= soupSize) {
        process->error = EXECUTE_OVERFLOW;
        return;
    }
    pattern[0] = soup[position + 1];
    pattern[1] = soup[position + 2];
    pattern[2] = soup[position + 3];
    pattern[3] = soup[position + 4];
    foundPosition = FindTemplate(pattern, position, BACKWARD, process);
    if (foundPosition == -1) {
        process->error = JUMP_TARGET_NOT_FOUND;
        return;
    }

    position = foundPosition;
    return position;
}

int VM::Command_Call(int position, Process *process) {
    char pattern[4];
    if (position + 4 >= soupSize) {
        process->error = EXECUTE_OVERFLOW;
        return;
    }
    pattern[0] = soup[position + 1];
    pattern[1] = soup[position + 2];
    pattern[2] = soup[position + 3];
    pattern[3] = soup[position + 4];
    foundPosition = FindTemplate(pattern, position, NEAREST, process);
    if (foundPosition == -1) {
        process->error = JUMP_TARGET_NOT_FOUND;
        return;
    }
    Push(position + 1, process);
    position = foundPosition;
    return position;
}

int VM::Command_Ret(int position, Process *process) {
    position = Pop(process);
    return position;
}

int VM::Command_Mov_CD(int position, Process *process) {
    process->dx = process->cx;
    return ++position;
}

// move ax to bx, bx=ax
int VM::Command_Mov_AB(int position, Process *process) {
    process->bx = process->ax;
    return ++position;
}

// move instruction at [bx] to [ax], [ax]=[bx]
int VM::Command_Mov_IAB(int position, Process *process) {
    if (process->ax < process->startPoint || process->ax >= process->startPoint + process->size) {
        process->error = COPY_OVER_FLOW;
        return;
    }
    if (process->bx < process->daughterStartPoint || process->bx >= process->daughterStartPoint + process->daughterSize) {
        process->error = COPY_OVER_FLOW;
        return;
    }
    if (process->bx < 0 || process->ax < 0 || process->ax >= soupSize || process->bx >= soupSize) {
        process->error = COPY_OVER_FLOW;
        return;
    }
    soup[process->bx] = soup[process->ax];
    return ++position;
}

int VM::Command_Adr_(directionType direction, int position, Process *process) {
    char pattern[4];
    if (position + 4 >= soupSize) {
        process->error = EXECUTE_OVERFLOW;
        return;
    }
    pattern[0] = soup[position + 1];
    pattern[1] = soup[position + 2];
    pattern[2] = soup[position + 3];
    pattern[3] = soup[position + 4];
    process->ax = FindTemplate(pattern, position, direction, process);
    return ++position;
}

// 	search template (nearest) address to ax
int VM::Command_Adr(int position, Process *process) {
    return Command_Adr_(NEAREST, position, process);
}

// search template (backward) address to ax
int VM::Command_AdrB(int position, Process *process) {
    return Command_Adr_(BACKWARD, position, process);
}

// search template (forward) address to ax
int VM::Command_AdrF(int position, Process *process) {
    return Command_Adr_(FORWARD, position, process);
}

// 	allocate memory, cx=size, return address in ax
int VM::Command_Mal(int position, Process *process) {

}

int VM::Command_Divide(int position, Process *process) {

}

int VM::ExecuteCommand(int position, Process *process) {
    switch (soup[position]) {
        case 0x00: // nop_0
        case 0x01: // nop_1
            return Command_Nop(position, process);
            break;
        case 0x02: // or1
            return Command_Or1(position, process);
        case 0x03: // shl
            return Command_Shl(position, process);
        case 0x04: // zero
            return Command_Shl(position, process);
        case 0x05: // if_cz
            return Command_If_Cz(position, process);
        case 0x06: // sub_ab
            return Command_Sub_Ab(position, process);
        case 0x07: // sub_ab
            return Command_Sub_Ac(position, process);
        case 0x08:
            return Command_Inc_A(position, process);
        case 0x09:
            return Command_Inc_B(position, process);
        case 0x0a:
            return Command_Dec_C(position, process);
        case 0x0b:
            return Command_Inc_C(position, process);
        case 0x0c:
            return Command_Push_Ax(position, process);
        case 0x0d:
            return Command_Push_Bx(position, process);
        case 0x0e:
            return Command_Push_Cx(position, process);
        case 0x0f:
            return Command_Push_Dx(position, process);
        case 0x10:
            return Command_Pop_Ax(position, process);
        case 0x11:
            return Command_Pop_Bx(position, process);
        case 0x12:
            return Command_Pop_Cx(position, process);
        case 0x13:
            return Command_Pop_Dx(position, process);
        case 0x14:
            return Command_Jmp(position, process);
        case 0x15:
            return Command_JmpB(position, process);
        case 0x16:
            return Command_Call(position, process);
        case 0x17:
            return Command_Ret(position, process);
        case 0x18:
            return Command_Mov_CD(position, process);
        case 0x19:
            return Command_Mov_AB(position, process);
        case 0x1a:
            return Command_Mov_IAB(position, process);
        case 0x1b:
            return Command_Adr(position, process);
        case 0x1c:
            return Command_AdrB(position, process);
        case 0x1d:
            return Command_AdrF(position, process);
        case 0x1e:
            return Command_Mal(position, process);
        case 0x1f:
            divide();
            break; //
    }
}
