const int pinX = A0;
const int pinY = A1;
const int pinSW = 2;

int centerX = 512; // valeurs par défaut, écrasées par la calibration
int centerY = 512;
const int deadzone = 15;

void setup() {
  pinMode(pinSW, INPUT_PULLUP);
  Serial.begin(9600);

  // Calibration : moyenne sur 100 lectures (~1 seconde)
  long sumX = 0, sumY = 0;
  const int samples = 100;
  for (int i = 0; i < samples; i++) {
    sumX += analogRead(pinX);
    sumY += analogRead(pinY);
    delay(10);
  }
  centerX = sumX / samples;
  centerY = sumY / samples;

  Serial.print("Calibration - centerX: "); Serial.print(centerX);
  Serial.print(" centerY: "); Serial.println(centerY);
}

void loop() {
  int rawX = analogRead(pinX) - centerX;
  int rawY = analogRead(pinY) - centerY;

  if (abs(rawX) < deadzone) rawX = 0;
  if (abs(rawY) < deadzone) rawY = 0;

  bool pressed = !digitalRead(pinSW);

  Serial.print("X: "); Serial.print(rawX);
  Serial.print(" Y: "); Serial.print(rawY);
  Serial.print(" SW: "); Serial.println(pressed);
  delay(100);
}