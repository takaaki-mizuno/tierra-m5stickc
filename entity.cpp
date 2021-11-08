//
// Created by Takaaki Mizuno on 2021/09/10.
//
#include <stddef.h>
#include "entity.h"
#include "MD5.h"

Entity::Entity(int startPoint, int size, char *entityPosition, unsigned long startTime) {
    this->startPoint = startPoint;
    this->size = size;
    this->next = NULL;
    this->startTime = startTime;

    unsigned char *rawHash = MD5::make_hash(entityPosition, size);
    hash = MD5::make_digest(rawHash, 16);
    free(rawHash);
}

Entity::~Entity() {
    free(hash);
}

