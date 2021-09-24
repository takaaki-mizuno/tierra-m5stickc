//
// Created by Takaaki Mizuno on 2021/08/25.
//

#ifndef CONCEPTUAL_VIRUS_VM_H
#define CONCEPTUAL_VIRUS_VM_H

#include "process.h"
#include "entity.h"

enum errorType {
    NO_ERROR = 0,
    STACK_OVER_FLOW,
    STACK_UNDER_FLOW,
    EXECUTE_OVERFLOW,
    JUMP_TARGET_NOT_FOUND,
    COPY_OVER_FLOW,
};

enum directionType {
    BACKWARD = -1,
    NEAREST = 0,
    FORWARD = 1
};

const int TEMPLATE_SEARCH_RANGE = 300;

class VM {
private:
    char *soup;
    Entity *entities;
    int soupSize;
    int nextPosition;
public:
    VM(int size);
    ~VM();
    void introduceFragment(char *fragment, int length);
    void allocateMemory(int size, Process *process);
    void Execute(Entity *entry);
    void Push(int value, Process *process);
    int Pop(Process *process);
    bool MatchPattern(char *pattern, int position, Process *process);
    int FindTemplate(char *pattern, int position, directionType direction, Process *process);
    int Command_Nop(int position, Process *process);
    int Command_Or1(int position, Process *process);
    int Command_Shl(int position, Process *process);
    int Command_Zero(int position, Process *process);
    int Command_If_Cz(int position, Process *process);
    int Command_Sub_Ab(int position, Process *process);
    int Command_Sub_Ac(int position, Process *process);
    int Command_Inc_A(int position, Process *process);
    int Command_Inc_B(int position, Process *process);
    int Command_Dec_C(int position, Process *process);
    int Command_Inc_C(int position, Process *process);
    int Command_Push_Ax(int position, Process *process);
    int Command_Push_Bx(int position, Process *process);
    int Command_Push_Cx(int position, Process *process);
    int Command_Push_Dx(int position, Process *process);
    int Command_Pop_Ax(int position, Process *process);
    int Command_Pop_Bx(int position, Process *process);
    int Command_Pop_Cx(int position, Process *process);
    int Command_Pop_Dx(int position, Process *process);
    int Command_Jmp(int position, Process *process);
    int Command_JmpB(int position, Process *process);
    int Command_Call(int position, Process *process);
    int Command_Ret(int position, Process *process);
    int Command_Mov_CD(int position, Process *process);
    int Command_Mov_AB(int position, Process *process);
    int Command_Mov_IAB(int position, Process *process);
    int Command_Adr_(directionType direction, int position, Process *process);
    int Command_Adr(int position, Process *process);
    int Command_AdrB(int position, Process *process);
    int Command_AdrF(int position, Process *process);
    int Command_Mal(int position, Process *process);
    int Command_Divide(int position, Process *process);
    int ExecuteCommand(int position, Process *process);
};

#endif //CONCEPTUAL_VIRUS_VM_H
