#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN          10
#define RST_PIN         9
#define LED_PIN_GREEN   7
#define BUZZER_PIN      6
#define LED_PIN_RED     5

MFRC522 rfid(SS_PIN, RST_PIN);

enum Badge { INCONNU, ADMIN, FAB, CHLOE, PAPA };

const byte UID_BADGE1[] = {0xB7, 0xFA, 0xBC, 0x60};  // administrateur

const byte UID_FAB[]    = {0x3C, 0xBD, 0xB2, 0xF4};
const byte UID_CHLOE[]  = {0xAC, 0x60, 0xB3, 0xF4};
const byte UID_PAPA[]  = {0x42, 0xc0, 0x40, 0x3d};


bool uidEgal(const byte *ref, byte taille) {
  if (rfid.uid.size != taille) return false;
  for (byte i = 0; i < taille; i++) {
    if (rfid.uid.uidByte[i] != ref[i]) return false;
  }
  return true;
}

Badge identifierBadge() {
  if (uidEgal(UID_BADGE1, sizeof(UID_BADGE1))) return ADMIN;
  if (uidEgal(UID_FAB,    sizeof(UID_FAB)))    return FAB;
  if (uidEgal(UID_CHLOE,  sizeof(UID_CHLOE)))  return CHLOE;
  if (uidEgal(UID_PAPA,  sizeof(UID_PAPA)))  return PAPA;
  else  return INCONNU;
}

void accesAutorise() {
  digitalWrite(LED_PIN_GREEN, HIGH);
  tone(BUZZER_PIN, 2000, 150);
  delay(200);
  tone(BUZZER_PIN, 2600, 250);
  delay(1000);
  digitalWrite(LED_PIN_GREEN, LOW);
}

void accesRefuse() {
  digitalWrite(LED_PIN_RED, HIGH);
  tone(BUZZER_PIN, 300, 400);
  delay(500);
  digitalWrite(LED_PIN_RED, LOW);
}

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();
  pinMode(LED_PIN_GREEN, OUTPUT);
  digitalWrite(LED_PIN_GREEN, LOW);
  pinMode(LED_PIN_RED, OUTPUT);
  digitalWrite(LED_PIN_RED, LOW);
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  switch (identifierBadge()) {
    case ADMIN:
      Serial.println("Bonjour Administrateur - acces autorise");
      accesAutorise();
      break;

    case FAB:
      Serial.println("Bonjour Fab - acces refuse");
      accesRefuse();
      break;

    case CHLOE:
      Serial.println("Bonjour Chloe - acces refuse");
      accesRefuse();
      break;

    case PAPA:
      Serial.println("Bonjour Jacques - acces autorise");
      accesAutorise();
      break;

    default:
      Serial.print("Badge inconnu, UID : ");
      for (byte i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i], HEX);
        Serial.print(' ');
      }
      Serial.println();
      accesRefuse();
      break;
  }

  rfid.PICC_HaltA();
}