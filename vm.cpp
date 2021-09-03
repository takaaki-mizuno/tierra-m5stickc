#include "vm.h"


VM::VM(int size) {
    soupSize = size;
    soup = new char[size];
    for (int i = 0; i < size; i++) {
        soup[i] = 0;
    }
}

VM::~VM() {
    if soup != null{
                delete[] soup;
        }
}

void VM::introduceFragment(char *fragment, int length) {

}

void VM::Execute(int startPosition) {
    int currentPosition = startPosition;
    cpuStruct cpu;
    cpuData.ax = 0;

}

void VM::Push(int value, cpuStruct *cpu) {
    if (cpu->sp >= 10) {
        cpu->error = STACK_OVER_FLOW;
        return;
    }
    cpu->stack[cpu->sp++] = value;
    return;
}

int VM::Pop(cpuStruct *cpu) {
    if (cpu->sp <= 0 >) {
        cpu->error = STACK_UNDER_FLOW;
        return;
    }
    int val = cpu->stack[--cpu->sp];
    cpu->stack[cpu->sp] = 0;
    return val;
}

bool VM::MatchPattern(char *pattern, int position, cpuStruct *cpu) {
    return (pattern[0] == soup[position] &&
            pattern[1] == soup[position + 1] &&
            pattern[2] == soup[position + 2] &&
            pattern[3] == soup[position + 3])
}

int VM::FindTemplate(char *pattern, int position, directionType direction, cpuStruct *cpu) {
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
                if (MatchPattern(pattern, position, cpu)) {
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
                if (MatchPattern(pattern, position, cpu)) {
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

int VM::Command_Nop(int position, cpuStruct *cpu) {
    return ++position;
}

int VM::Command_Or1(int position, cpuStruct *cpu) {
    cpu->cx ^= 1;
    return ++position;
}

int VM::Command_Shl(int position, cpuStruct *cpu) {
    cpu->cx <<= 1;
    return ++position;
}

int VM::Command_Zero(int position, cpuStruct *cpu) {
    cpu->cx = 0;
    return ++position;
}

int VM::Command_If_Cz(int position, cpuStruct *cpu) {
    if (cpu->cx != 0) {
        position++;
    }
    return ++position;
}

int VM::Command_Sub_Ab(int position, cpuStruct *cpu) {
    cpu->cx = cpu->ax - cpu->bx;
    return ++position;
}

int VM::Command_Sub_Ac(int position, cpuStruct *cpu) {
    cpu->ax = cpu->ax - cpu->cx;
    return ++position;
}

int VM::Command_Inc_A(int position, cpuStruct *cpu) {
    cpu->ax++;
    return ++position;
}

int VM::Command_Inc_B(int position, cpuStruct *cpu) {
    cpu->bx++;
    return ++position;
}

int VM::Command_Dec_C(int position, cpuStruct *cpu) {
    cpu->cx--;
    return ++position;
}

int VM::Command_Inc_C(int position, cpuStruct *cpu) {
    cpu->cx++;
    return ++position;
}

int VM::Command_Push_Ax(int position, cpuStruct *cpu) {
    Push(cpu->ax, cpu);
    return ++position;
}

int VM::Command_Push_Bx(int position, cpuStruct *cpu) {
    Push(cpu->bx, cpu);
    return ++position;
}

int VM::Command_Push_Cx(int position, cpuStruct *cpu) {
    Push(cpu->cx, cpu);
    return ++position;
}

int VM::Command_Push_Dx(int position, cpuStruct *cpu) {
    Push(cpu->dx, cpu);
    return ++position;
}

int VM::Command_Pop_Ax(int position, cpuStruct *cpu) {
    cpu->ax = Pop(cpu);
    return ++position;
}

int VM::Command_Pop_Bx(int position, cpuStruct *cpu) {
    cpu->bx = Pop(cpu);
    return ++position;
}

int VM::Command_Pop_Cx(int position, cpuStruct *cpu) {
    cpu->cx = Pop(cpu);
    return ++position;
}

int VM::Command_Pop_Dx(int position, cpuStruct *cpu) {
    cpu->dx = Pop(cpu);
    return ++position;
}

int VM::Command_Jmp(int position, cpuStruct *cpu) {
    char pattern[4];
    if (position + 4 >= soupSize) {
        cpu->error = EXECUTE_OVERFLOW;
        return;
    }
    pattern[0] = soup[position + 1];
    pattern[1] = soup[position + 2];
    pattern[2] = soup[position + 3];
    pattern[3] = soup[position + 4];
    foundPosition = FindTemplate(pattern, position, NEAREST, cpu);
    if (foundPosition == -1) {
        cpu->error = JUMP_TARGET_NOT_FOUND;
        return;
    }

    position = foundPosition;
    return position;
}

int VM::Command_JmpB(int position, cpuStruct *cpu) {
    char pattern[4];
    if (position + 4 >= soupSize) {
        cpu->error = EXECUTE_OVERFLOW;
        return;
    }
    pattern[0] = soup[position + 1];
    pattern[1] = soup[position + 2];
    pattern[2] = soup[position + 3];
    pattern[3] = soup[position + 4];
    foundPosition = FindTemplate(pattern, position, BACKWARD, cpu);
    if (foundPosition == -1) {
        cpu->error = JUMP_TARGET_NOT_FOUND;
        return;
    }

    position = foundPosition;
    return position;
}

int VM::Command_Call(int position, cpuStruct *cpu) {
    char pattern[4];
    if (position + 4 >= soupSize) {
        cpu->error = EXECUTE_OVERFLOW;
        return;
    }
    pattern[0] = soup[position + 1];
    pattern[1] = soup[position + 2];
    pattern[2] = soup[position + 3];
    pattern[3] = soup[position + 4];
    foundPosition = FindTemplate(pattern, position, NEAREST, cpu);
    if (foundPosition == -1) {
        cpu->error = JUMP_TARGET_NOT_FOUND;
        return;
    }
    Push(position+1, cpu);
    position = foundPosition;
    return position;
}

int VM::Command_Ret(int position, cpuStruct *cpu) {
    position = Pop(cpu);
    return position;
}

int VM::    Command_Mov_CD(int position, cpuStruct *cpu) {
    cpu->dx = cpu->cx;
    return ++position;
}

int VM::Command_Mov_AB(int position, cpuStruct *cpu) {
    cpu->bx = cpu->ax;
    return ++position;
}

int VM::Command_Mov_IAB(int position, cpuStruct *cpu) {
    cpu->bx = cpu->ax;
    return ++position;
}

int VM::ExecuteCommand(int position, cpuStruct *cpu) {
    switch (soup[position]) {
        case 0x00: // nop_0
        case 0x01: // nop_1
            return Command_Nop(position, cpu);
            break;
        case 0x02: // or1
            return Command_Or1(position, cpu);
        case 0x03: // shl
            return Command_Shl(position, cpu);
        case 0x04: // zero
            return Command_Shl(position, cpu);
        case 0x05: // if_cz
            return Command_If_Cz(position, cpu);
        case 0x06: // sub_ab
            return Command_Sub_Ab(position, cpu);
        case 0x07: // sub_ab
            return Command_Sub_Ac(position, cpu);
        case 0x08:
            return Command_Inc_A(position, cpu);
        case 0x09:
            return Command_Inc_B(position, cpu);
        case 0x0a:
            return Command_Dec_C(position, cpu);
        case 0x0b:
            return Command_Inc_C(position, cpu);
        case 0x0c:
            return Command_Push_Ax(position, cpu);
        case 0x0d:
            return Command_Push_Bx(position, cpu);
        case 0x0e:
            return Command_Push_Cx(position, cpu);
        case 0x0f:
            return Command_Push_Dx(position, cpu);
        case 0x10:
            return Command_Pop_Ax(position, cpu);
        case 0x11:
            return Command_Pop_Bx(position, cpu);
        case 0x12:
            return Command_Pop_Cx(position, cpu);
        case 0x13:
            return Command_Pop_Dx(position, cpu);
        case 0x14:
            return Command_Jmp(position, cpu);
            break;
        case 0x15:
            return Command_JmpB(position, cpu);
            break;
        case 0x16:
            return Command_Call(position, cpu);
        case 0x17:
            return Command_Ret(position, cpu);
        case 0x18:
            return Command_Mov_CD(position, cpu);
        case 0x19:
            return Command_Mov_AB(position, cpu);
        case 0x1a:
            mov_iab();	break; //
            case 0x1b: adr();		break; //
            case 0x1c: adrb();		break; //
            case 0x1d: adrf();		break; //
            case 0x1e: mal();		break; //
            case 0x1f: divide();	break; //


    }
}
