//
// Created by Takaaki Mizuno on 2021/08/25.
//

#ifndef CONCEPTUAL_VIRUS_VM_H
#define CONCEPTUAL_VIRUS_VM_H

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
    Entity []*entities;
    int soupSize;
    int nextPosition;
public:
    VM(int size);

    ~VM();

    introduceFragment(char *fragment, int length);
};

#endif //CONCEPTUAL_VIRUS_VM_H
