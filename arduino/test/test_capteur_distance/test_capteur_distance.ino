
const int TRIG = 11;
const int ECHO = 12;

float tempC = 26.0;                  // à régler à la main au départ
float v = 331.3 + 0.606 * tempC;     // vitesse du son en m/s

unsigned long mesureUs() {
  digitalWrite(TRIG, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  return pulseIn(ECHO, HIGH, 30000);  // µs, 0 si timeout
}

void calibrer(float refCm) {
  if (refCm < 10) return;
  unsigned long somme = 0;
  int n = 0;
  for (int i = 0; i < 20; i++) {
    unsigned long t = mesureUs();
    if (t > 0) { somme += t; n++; }
    delay(60);
  }
  if (n < 10) { Serial.println("calibration impossible"); return; }
  float tMoy = (float)somme / n;                   // µs
  v = (refCm / 100.0) * 2.0 / (tMoy * 1e-6);       // m/s
  Serial.print("v calibree = ");
  Serial.print(v, 1);
  Serial.println(" m/s");
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  Serial.println("Calibration : envoie c100 avec une cible a 100 cm");
}

void loop() {
  if (Serial.available() && Serial.read() == 'c') {
    calibrer(Serial.parseFloat());     // distance réelle en cm
  }

  unsigned long dt = mesureUs();
  if (dt == 0) {
    Serial.println("hors portée");
  } else {
    float d = dt * v * 0.00005;        // cm
    Serial.print(d, 1);
    Serial.println(" cm");
  }
  delay(60);
}