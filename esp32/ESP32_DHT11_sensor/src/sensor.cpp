#include <Arduino.h>
#include <DHT.h>
#include "sensor.h"

#define DHTPIN 4
#define DHTTYPE DHT11
#define READ_INTERVAL_MS 2000

static DHT dht(DHTPIN, DHTTYPE);
static Reading current = {0, 0, false};

void sensorBegin() {
  dht.begin();
}

void sensorUpdate() {
  static unsigned long last = 0;
  if (millis() - last < READ_INTERVAL_MS) return;
  last = millis();

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    current.valid = false;
    return;
  }
  current = {t, h, true};
}

Reading sensorGet() {
  return current;
}