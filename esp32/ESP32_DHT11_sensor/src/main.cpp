#include <Arduino.h>
#include "wifi_manager.h"
#include "sensor.h"
#include "web_server.h"

void setup() {
  Serial.begin(115200);
  sensorBegin();
  wifiConnect();
  webBegin();
}

void loop() {
  wifiCheck();
  sensorUpdate();
  webHandle();
}