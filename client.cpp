//
// Created by Takaaki Mizuno on 2021/11/06.
//
#include <M5StickC.h>
#include <ArduinoJson.h>
#include "client.h"
#include "status.h"
#include "config.h"
#include <HTTPClient.h>
#include <WiFi.h>

const char *updateUrl = "http://10.202.167.127:8100/api/creatures/status";

char buffer[6500];

DashboardClient::DashboardClient(VM *_vm) {
    vm = _vm;
}

DashboardClient::~DashboardClient() {
}

void DashboardClient::BuildJson(char *position, int size) {
    char* currentPosition = position;
    int rest = size;
    int n = snprintf(currentPosition, rest, "{\"i\":\"%d.%d.%d.%d\",\"k\":\"%s\",\"s\":[",
                     WiFi.localIP()[0],
                     WiFi.localIP()[1],
                     WiFi.localIP()[2],
                     WiFi.localIP()[3],
                     vm->GetID());

    if (n < 0) {
        position[size - 1] = '\0';
        return;
    }

    rest -= n;
    currentPosition += n;
    Status status[100];
    int count = vm->GetStatus(status, 100);

    for (int i = 0; i < count; i++) {
        int n = snprintf(currentPosition, rest, "{\"i\":%d,\"h\":\"%s\"},",
                         status[i].index,
                         status[i].hash);
        if (n < 0) {
            position[size - 1] = '\0';
            return;
        }
        rest -= n;
        currentPosition += n;
    }
    rest++;
    currentPosition--;
    if (rest >= 3) {
        *currentPosition++ = ']';
        *currentPosition++ = '}';
        *currentPosition++ = '\0';
    } else {
        if (rest > 0) {
            *currentPosition++ = '\0';
        } else {
            position[size - 1] = '\0';
        }
    }
}

void DashboardClient::SendInfo() {
    HTTPClient http;

    BuildJson(buffer, 6500);
//    Serial.print(buffer);

    http.begin(updateUrl);
    http.addHeader("Content-Type", "application/json");
    int status_code = http.POST((uint8_t *)buffer, strlen(buffer));
    Serial.printf("status_code=%d\r\n", status_code);
    http.end();
}

char* DashboardClient::GetStatusJSON() {
    BuildJson(buffer, 6500);
    return buffer;
}
