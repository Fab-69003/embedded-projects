#include <Servo.h>

Servo monServo;
const int POS_AXE = 0;   // valeur write() quand le servo est dans l'axe

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);

  monServo.attach(9);
  monServo.write(POS_AXE);
  Serial.println("Entre un angle par rapport a l'axe (0 = dans l'axe) :");
}

void loop() {
  if (Serial.available()) {
    String saisie = Serial.readStringUntil('\n');
    saisie.trim();
    if (saisie.length() == 0) return;

    int angle = saisie.toInt();
    int reel = constrain(POS_AXE + angle, 0, 180);
    monServo.write(reel);

    Serial.print("Angle demande : ");
    Serial.print(angle);
    Serial.print("  -> write(");
    Serial.print(reel);
    Serial.println(")");
  }
}