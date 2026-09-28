#include <Arduino.h>
#include <WebServer.h>
#include "web_server.h"
#include "sensor.h"

static WebServer server(80);

static const char PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 - Météo</title>
  <style>
    body { font-family: sans-serif; text-align: center; margin-top: 3rem; }
    .val { font-size: 3rem; margin: 1rem; }
  </style>
</head>
<body>
  <h1>ESP32 - DHT11</h1>
  <div class="val">🌡️ <span id="t">--</span> °C</div>
  <div class="val">💧 <span id="h">--</span> %</div>
  <script>
    async function refresh() {
      try {
        const r = await fetch('/data');
        const d = await r.json();
        document.getElementById('t').textContent = d.valid ? d.t.toFixed(1) : '--';
        document.getElementById('h').textContent = d.valid ? d.h.toFixed(0) : '--';
      } catch (e) {}
    }
    refresh();
    setInterval(refresh, 2000);
  </script>
</body>
</html>
)rawliteral";

static void handleRoot() {
  server.send_P(200, "text/html", PAGE);
}

static void handleData() {
  Reading r = sensorGet();
  String json = "{\"valid\":" + String(r.valid ? "true" : "false") +
                ",\"t\":" + String(r.temperature, 1) +
                ",\"h\":" + String(r.humidity, 1) + "}";
  server.send(200, "application/json", json);
}

void webBegin() {
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
}

void webHandle() {
  server.handleClient();
}