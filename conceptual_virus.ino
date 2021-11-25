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

    setupWifi();
    setupHttpServer();

    vm = new VM(randNumber);
    client = new DashboardClient(vm);
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
        client->SendInfo();
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
    server.on("/dump", handleDump);
    server.onNotFound(handleNotFound);
    server.begin();
}

void handleRoot() {
    server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleStatus() {
    char *buffer = client->GetStatusJSON();
    server.send(200, "application/json", buffer);
}

void handleTransport() {
    server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleDump() {
    if(!server.hasArg("index")){
        server.send(400, "application/json", "{\"status\":\"no index\"}");
        return;
    }
    int index = server.arg("index").toInt();
    char *dump = vm->DumpToChar(index);
    server.send(200, "application/json", dump);
    return;
}

void handleNotFound() {
    server.send(404, "text/plain", "File Not Found\n\n");
}
