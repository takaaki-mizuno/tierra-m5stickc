//
// Created by Takaaki Mizuno on 2021/08/25.
//

#ifndef ARTIFICIAL_LIFE_VM_H
#define ARTIFICIAL_LIFE_VM_H

struct cpuStruct {  /* structure for registers of virtual cpu */
    int ax;  /* address register */
    int bx;  /* address register */
    int cx;  /* numerical register */
    int dx;  /* numerical register */
    char fl;  /* flag */
    char sp;  /* stack pointer */
    int stack[10];  /* stack */
    int ip; /* instruction pointer */
    int error;
};

enum errorType {
    NO_ERROR = 0,
    STACK_OVER_FLOW,
    STACK_UNDER_FLOW,
    EXECUTE_OVERFLOW,
    JUMP_TARGET_NOT_FOUND,
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
    int soupSize;
public:
    VM(int size);

    ~VM();

    introduceFragment(char *fragment, int length);
};

#endif //ARTIFICIAL_LIFE_VM_H
