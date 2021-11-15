//
// Conceptual Virus for M5STickC
// Created by Takaaki Mizuno on 2021/09/10.
//

#ifndef CONCEPTUAL_VIRUS_ENTITY_H
#define CONCEPTUAL_VIRUS_ENTITY_H

#include "Arduino.h"


class Entity {
public:
    void SetValue(int startPoint, int size, char *entityPosition, unsigned long startTime, int index);
    void CalcHash();
    int CalcX();
    int CalcY();
    uint16_t CalcColor();
    int startPoint;
    char *entityPosition;
    int size;
    char hash[33];
    unsigned long startTime;
    bool active;
    int index;
};

#endif //CONCEPTUAL_VIRUS_ENTITY_H
