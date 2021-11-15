//
// Created by Takaaki Mizuno on 2021/09/10.
//
#include <stddef.h>
#include "entity.h"
#include "MD5.h"
#include <M5StickC.h>
#include "Arduino.h"

void Entity::SetValue(int startPoint, int size, char *entityPosition, unsigned long startTime, int index) {
    this->index = index;
    this->startPoint = startPoint;
    this->size = size;
    this->startTime = startTime;
    this->active = true;
    this->entityPosition = entityPosition;
    CalcHash();
}

void Entity::CalcHash() {
    unsigned char *rawHash = MD5::make_hash(entityPosition, size);
    char *temp = MD5::make_digest(rawHash, 16);
    for (int i = 0; i < 32; i++) {
        hash[i] = temp[i];
    }
    hash[32] = 0;
    free(rawHash);
    free(temp);
    M5.Lcd.fillRect(this->CalcX(),
                    this->CalcY(),
                    15,
                    2,
                    this->CalcColor());
}

int Entity::CalcX() {
    int x = (this->index % 16 ) * 16;
    return x;
}

int Entity::CalcY() {
    int y = this->index / 16;
    return y * 2 + 30;
}

uint16_t Entity::CalcColor() {
    uint16_t color_value = M5.Lcd.color565((int8_t)hash[0],(int8_t)hash[1],(int8_t)hash[2]);
    return color_value;
}
