//
// Conceptual Virus for M5STickC
// Created by Takaaki Mizuno on 2021/09/10.
//

#ifndef CONCEPTUAL_VIRUS_ENTITY_H
#define CONCEPTUAL_VIRUS_ENTITY_H


class Entity {
public:
    Entity(int startPoint, int size);
    ~Entity();
    int startPoint;
    int size;
    Entity *next;
};


#endif //CONCEPTUAL_VIRUS_ENTITY_H
