#include <Arduino.h>
#include <WiFi.h>
#include "wifi_manager.h"
#include "secrets.h"

void wifiConnect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connexion WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnecté, IP : ");
  Serial.println(WiFi.localIP());
}

void wifiCheck() {
  static unsigned long last = 0;
  if (WiFi.status() != WL_CONNECTED && millis() - last > 10000) {
    last = millis();
    Serial.println("WiFi perdu, reconnexion...");
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }
}