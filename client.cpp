//
// Created by Takaaki Mizuno on 2021/11/06.
//
#include <M5StickC.h>
#include <ArduinoJson.h>
#include "client.h"
#include "status.h"
#include <HTTPClient.h>
#include <WiFi.h>

const char *updateUrl = "http://10.202.167.127:8100/api/creatures/status";

DashboardClient::DashboardClient(VM *_vm) {
    vm = _vm;
}

DashboardClient::~DashboardClient() {
}

void DashboardClient::SendInfo() {
    HTTPClient http;
    DynamicJsonDocument body(1024);

    body["ip_address"] = WiFi.localIP();
    body["identity_key"]   = 1351824120;
    Status status[100];
    int count = vm->GetStatus(status, 100);
    for( int i = 0; i < count; i++ ){
        body["status"][i]["index"] = status[i].index;
        body["status"][i]["length"] = status[i].length;
        body["status"][i]["life_time"] = status[i].life_time;
        body["status"][i]["hash"] = status[i].hash;
    }

    char buffer[1024];
    serializeJson(body, Serial);
    serializeJson(body, buffer, sizeof(buffer));

    http.begin(updateUrl);
    http.addHeader("Content-Type", "application/json");
    int status_code = http.POST((uint8_t*)buffer, strlen(buffer));
    Serial.printf("status_code=%d\r\n", status_code);
    http.end();
}