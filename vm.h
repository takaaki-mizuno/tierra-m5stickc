//
// Created by Takaaki Mizuno on 2021/08/25.
//

#ifndef CONCEPTUAL_VIRUS_VM_H
#define CONCEPTUAL_VIRUS_VM_H

#include "process.h"
#include "entity.h"
#include "status.h"
#include "config.h"
#include "Arduino.h"

enum errorType {
    NO_ERROR = 0,
    STACK_OVER_FLOW,
    STACK_UNDER_FLOW,
    EXECUTE_OVERFLOW,
    JUMP_TARGET_NOT_FOUND,
    COPY_OVER_FLOW_MOTHER,
    COPY_OVER_FLOW_DAUGHTER,
    COPY_OVER_FLOW_SOUP,
    CAN_NOT_MEMORY_ALLOCATION,
    UNKNOWN_CODE,
    NOT_PROCEED,
    TOO_MANY_STEP,
    ENTITY_ALLOCATION_ERROR,
    COMPLETE
};

enum directionType {
    BACKWARD = -1,
    NEAREST = 0,
    FORWARD = 1
};



class VM {
private:
    char soup[SOUP_SIZE];
    Entity entities[ENTITY_MAX_COUNT];
    int soupSize;
    int nextPosition;
    Process process;
    char *id;
    bool crashed;
    int totalExecutedInstructions;
public:
    VM(long seed);
    ~VM();
    void Dump(int start, int size);
    char* DumpToChar(int index);
    bool isCrashed();
    void DumpSoup();
    char* GetID();
    int GetStatus(Status statusList[], int max);
    void CreateID(long seed);
    void CleanEntities();
    void CopyCreature(char *source, char *destination, int length);
    int FindEntitySlot();
    int GetEntity(int size);
    int AllocateMemory(int size);
    int FindEmptySpace(int size, bool debug);
    void DeleteEntity(int i);
    void OneLifeCycle();
    void IntroduceMutation(int index);
    int Execute(int index, unsigned long startTime, bool debug);
    void Push(int value);
    int Pop();
    bool MatchPattern(char *pattern, int position);
    int FindTemplate(char *pattern, int position, directionType direction);
    int Command_Nop(int position);
    int Command_Or1(int position);
    int Command_Shl(int position);
    int Command_Zero(int position);
    int Command_If_Cz(int position);
    int Command_Sub_Ab(int position);
    int Command_Sub_Ac(int position);
    int Command_Inc_A(int position);
    int Command_Inc_B(int position);
    int Command_Dec_C(int position);
    int Command_Inc_C(int position);
    int Command_Push_Ax(int position);
    int Command_Push_Bx(int position);
    int Command_Push_Cx(int position);
    int Command_Push_Dx(int position);
    int Command_Pop_Ax(int position);
    int Command_Pop_Bx(int position);
    int Command_Pop_Cx(int position);
    int Command_Pop_Dx(int position);
    int Command_Jmp(int position);
    int Command_JmpB(int position);
    int Command_Call(int position);
    int Command_Ret(int position);
    int Command_Mov_CD(int position);
    int Command_Mov_AB(int position);
    void CleanUpDaughter();
    int Command_Mov_IAB(int position);
    int Command_Adr_(directionType direction, int position);
    int Command_Adr(int position);
    int Command_AdrB(int position);
    int Command_AdrF(int position);
    int Command_Mal(int position);
    int Command_Divide(int position);
    int ExecuteCommand(int position);
};

#endif //CONCEPTUAL_VIRUS_VM_H
