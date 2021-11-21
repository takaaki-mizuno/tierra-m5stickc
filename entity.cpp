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
    this->DrawOnScreen(false);
}

int Entity::CalcX() {
    int x = (this->startPoint % 640) / 4;
    return x;
}

int Entity::CalcY() {
    int y = this->startPoint / 640;
    return y * 2 + 20;
}

int Entity::CalcWidth() {
    int width = this->size / 4;
    return width;
}

uint16_t Entity::CalcColor() {
    uint16_t color_value = M5.Lcd.color565((int8_t) hash[1], (int8_t) hash[2], (int8_t) hash[3]);
    return color_value;
}

void Entity::DrawOnScreen(bool isDelete) {
    int x = this->CalcX();
    int y = this->CalcY();
    int width = this->CalcWidth();
    uint16_t color = this->CalcColor();
    if (isDelete) {
        color = BLACK;
    }
    if (x + width <= 160) {
        M5.Lcd.fillRect(x, y, width, 2, color);
    } else {
        M5.Lcd.fillRect(x, y, 160 - x, 2, color);
        M5.Lcd.fillRect(x, y, width + x - 160, 2, color);
    }
}
