//
// Created by Takaaki Mizuno on 2021/09/10.
//

#ifndef CONCEPTUAL_VIRUS_PROCESS_H
#define CONCEPTUAL_VIRUS_PROCESS_H


class Process {
public:
    Process(int startPoint, int size);
    ~Process();
    int ax;  /* address register */
    int bx;  /* address register */
    int cx;  /* numerical register */
    int dx;  /* numerical register */
    char fl;  /* flag */
    char sp;  /* stack pointer */
    int stack[10];  /* stack */
    int ip; /* instruction pointer */
    int error;
    int startPoint;
    int size;
    int daughterStartPoint;
    int daughterSize;
};


#endif //CONCEPTUAL_VIRUS_PROCESS_H