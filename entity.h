//
// Conceptual Virus for M5STickC
// Created by Takaaki Mizuno on 2021/09/10.
//

#ifndef CONCEPTUAL_VIRUS_ENTITY_H
#define CONCEPTUAL_VIRUS_ENTITY_H

class Entity {
public:
    void SetValue(int startPoint, int size, char *entityPosition, unsigned long startTime);
    int startPoint;
    int size;
    char hash[33];
    unsigned long startTime;
    bool active;
};

#endif //CONCEPTUAL_VIRUS_ENTITY_H
