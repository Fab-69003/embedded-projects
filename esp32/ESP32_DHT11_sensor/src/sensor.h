#pragma once

struct Reading {
  float temperature;
  float humidity;
  bool valid;
};

void sensorBegin();
void sensorUpdate();
Reading sensorGet();