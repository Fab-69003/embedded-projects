const int CLK = 2;
const int DT  = 3;
const int SW  = 4;

int position   = 0;
int lastCLK    = HIGH;

void setup() {
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT,  INPUT_PULLUP);
  pinMode(SW,  INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("Tourne l'encodeur !");
}

void loop() {
  int currentCLK = digitalRead(CLK);

  // Détection du front descendant de CLK
  if (currentCLK == LOW && lastCLK == HIGH) {
    if (digitalRead(DT) == HIGH) {
      position++;          // Sens horaire
      Serial.print("CW  → position : ");
    } else {
      position--;          // Anti-horaire
      Serial.print("CCW → position : ");
    }
    Serial.println(position);
    delay(5);              // Anti-rebond simple
  }

  lastCLK = currentCLK;

  // Bouton pressé
  if (digitalRead(SW) == LOW) {
    Serial.println("=== Bouton appuyé ! ===");
    position = 0;          // Remet à zéro
    Serial.println("Position remise à 0");
    delay(300);            // Anti-rebond bouton
  }
}