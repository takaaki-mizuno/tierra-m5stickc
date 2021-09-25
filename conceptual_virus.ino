#include <M5StickC.h>
#include <WiFi.h>
#include <WebServer.h>
#include "vm.h"

// wifiの設定
const char *ssid = "Buffalo-G-EFFE"; // WiFiのSSID
const char *password = ""; // WiFiのパスワード

WebServer server(80);
VM vm = VM();

void setup() {
    M5.begin();
    M5.Lcd.setRotation(1);
    M5.Axp.ScreenBreath(9);
    M5.Lcd.fillScreen(BLACK);
    setupWifi();
    setupLife();
    setupHttpServer();
}

void loop() {
    server.handleClient();
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
    server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleTransport() {
    server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleNotFound() {
    server.send(404, "text/plain", "File Not Found\n\n");
}

// ----------

void setupLife() {

}
