//
// Created by Takaaki Mizuno on 2021/09/10.
//
#include <stddef.h>
#include "entity.h"

Entity::Entity(int startPoint, int size) {
    this->startPoint = startPoint;
    this->size = size;
    this->next = NULL;
}

Entity::~Entity() {
}
