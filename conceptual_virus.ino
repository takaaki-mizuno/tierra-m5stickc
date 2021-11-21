#include <M5StickC.h>
#include <WiFi.h>
#include <WebServer.h>
#include "vm.h"
#include "client.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

// wifiの設定

//const char *ssid = ""; // WiFiのSSID
//const char *password = ""; // WiFiのパスワード

WebServer server(80);
VM* vm;
DashboardClient* client;

unsigned long previousExecution, interval;


void setup() {
    M5.begin();
    M5.Lcd.setRotation(1);
    M5.Axp.ScreenBreath(9);
    M5.Lcd.fillScreen(BLACK);

    interval = 1000 * 60 * 1; // 1 mi

    Serial.begin(9600);
    randomSeed(analogRead(0));
    long randNumber = random(100000000);

    vm = new VM(randNumber);
    client = new DashboardClient(vm);
    setupWifi();
    setupHttpServer();
    client->SendInfo();
    vm->OneLifeCycle();
}

void loop() {
    server.handleClient();
    unsigned long current = millis();
    if ((current - previousExecution) >= interval) {
        Serial.print("Execution");
        Serial.print(current);
        vm->OneLifeCycle();
        previousExecution = current;
    }
}

void setupWifi() {
    if (WiFi.status() != WL_CONNECTED) {
        M5.Lcd.print("WiFi connecting");
        WiFi.begin(ssid, password);

        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            M5.Lcd.print(".");
        }
    }
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0, 1);
    M5.Lcd.print("IP address: ");
    M5.Lcd.print(WiFi.localIP());
}

void setupHttpServer() {
    server.on("/", handleRoot);
    server.on("/status", handleStatus);
    server.on("/transport", handleTransport);
    server.onNotFound(handleNotFound);
    server.begin();
}

void handleRoot() {
    server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleStatus() {
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

    server.send(200, "application/json", buffer);
}

void handleTransport() {
    server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleNotFound() {
    server.send(404, "text/plain", "File Not Found\n\n");
}
