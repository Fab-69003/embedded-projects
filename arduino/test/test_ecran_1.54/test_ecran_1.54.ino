#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

#define OLED_ADDR 0x3C

// Efface les 132 colonnes de RAM (U8g2 n'en écrit que 128)
void clearFullRam() {
  for (uint8_t page = 0; page < 8; page++) {
    Wire.beginTransmission(OLED_ADDR);
    Wire.write(0x00);          // octet de contrôle : commande
    Wire.write(0xB0 + page);   // sélection de la page
    Wire.write(0x00);          // colonne basse = 0
    Wire.write(0x10);          // colonne haute = 0
    Wire.endTransmission();

    // 132 octets à zéro, par paquets de 16
    for (uint8_t col = 0; col < 132; col += 16) {
      Wire.beginTransmission(OLED_ADDR);
      Wire.write(0x40);        // octet de contrôle : données
      for (uint8_t i = 0; i < 16 && (col + i) < 132; i++) {
        Wire.write(0x00);
      }
      Wire.endTransmission();
    }
  }
}
// Affiche un texte en le découpant automatiquement en lignes
void afficherTexte(String texte) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tf);   // 6 px de large, ~21 caractères par ligne

  const uint8_t maxChars = 21;
  const uint8_t hauteurLigne = 12;
  uint8_t y = 12;
  uint8_t debut = 0;

  while (debut < texte.length() && y <= 64) {
    String ligne = texte.substring(debut, min((unsigned int)(debut + maxChars), texte.length()));
    u8g2.drawStr(0, y, ligne.c_str());
    debut += maxChars;
    y += hauteurLigne;
  }

  u8g2.sendBuffer();
}

void setup() {
  Serial.begin(9600);
  u8g2.begin();
  clearFullRam();

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.drawStr(10, 25, "Arduino");
  u8g2.drawStr(10, 50, "UNO R4 !");
  u8g2.sendBuffer();

  delay(250);

  afficherTexte("Tape un message...");
  Serial.println("Tape un message et valide avec Entree :");
}

void loop() {
  if (Serial.available()) {
    String message = Serial.readStringUntil('\n');
    message.trim();

    if (message.length() > 0) {
      afficherTexte(message);
      Serial.print("Affiche : ");
      Serial.println(message);
    }
  }
}