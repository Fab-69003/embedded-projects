
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

const int pinY = A1;   // VRy -> déplacement vertical de la raquette
const int pinSW = 2;   // bouton du joystick -> start / restart

const int paddleWidth = 4;
const int paddleHeight = 16;
const int paddleX = SCREEN_WIDTH - paddleWidth - 2;

float ballX, ballY;
float ballDX, ballDY;
int paddleY;
int score;

enum GameState { WAITING, PLAYING, GAME_OVER };
GameState state = WAITING;

bool lastSW = HIGH;

void clearFullRam() {
  for (uint8_t page = 0; page < 8; page++) {
    Wire.beginTransmission(OLED_ADDR);
    Wire.write(0x00); Wire.write(0xB0 + page);
    Wire.write(0x00); Wire.write(0x10);
    Wire.endTransmission();
    for (uint8_t col = 0; col < 132; col += 16) {
      Wire.beginTransmission(OLED_ADDR);
      Wire.write(0x40);
      for (uint8_t i = 0; i < 16 && (col + i) < 132; i++) Wire.write(0x00);
      Wire.endTransmission();
    }
  }
}

void resetBall() {
  ballX = SCREEN_WIDTH / 2;
  ballY = random(10, SCREEN_HEIGHT - 10);
  ballDX = -2;
  ballDY = (random(0, 2) == 0) ? 1.5 : -1.5;
}

void setup() {
  pinMode(pinSW, INPUT_PULLUP);
  u8g2.begin();   // initialise aussi le bus I2C, pas besoin de Wire.begin() séparé
  clearFullRam();
  randomSeed(analogRead(A0));
  score = 0;
  resetBall();
}

bool swPressed() {
  bool sw = digitalRead(pinSW);
  bool pressed = (lastSW == HIGH && sw == LOW);
  lastSW = sw;
  return pressed;
}

void drawWaiting() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_te);
  u8g2.drawUTF8(10, 20, "PONG SOLO");
  u8g2.drawUTF8(5, 40, "Appuie SW pour jouer");
  u8g2.sendBuffer();
}

void drawGameOver() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_te);
  u8g2.drawUTF8(20, 15, "GAME OVER");
  String scoreTxt = "Score: " + String(score);
  u8g2.drawUTF8(20, 30, scoreTxt.c_str());
  u8g2.drawUTF8(5, 50, "SW pour rejouer");
  u8g2.sendBuffer();
}

void updateGame() {
  // Raquette pilotée par VRy
  int raw = analogRead(pinY);
  paddleY = map(raw, 0, 1023, 0, SCREEN_HEIGHT - paddleHeight);

  // Déplacement de la balle
  ballX += ballDX;
  ballY += ballDY;

  // Rebond haut/bas
  if (ballY <= 0 || ballY >= SCREEN_HEIGHT - 2) {
    ballDY = -ballDY;
  }

  // Rebond mur gauche
  if (ballX <= 0) {
    ballDX = -ballDX;
  }

  // Zone de la raquette (à droite)
  if (ballX >= paddleX - 2) {
    if (ballY >= paddleY && ballY <= paddleY + paddleHeight) {
      ballDX = -ballDX;
      score++;
    } else if (ballX >= SCREEN_WIDTH) {
      state = GAME_OVER;
    }
  }
}

void drawGame() {
  u8g2.clearBuffer();
  u8g2.drawBox(paddleX, paddleY, paddleWidth, paddleHeight);
  u8g2.drawBox((int)ballX, (int)ballY, 3, 3);
  u8g2.setFont(u8g2_font_6x12_te);
  u8g2.setCursor(0, 10);
  u8g2.print(score);
  u8g2.sendBuffer();
}

void loop() {
  bool pressed = swPressed();

  switch (state) {
    case WAITING:
      drawWaiting();
      if (pressed) {
        score = 0;
        resetBall();
        state = PLAYING;
      }
      break;

    case PLAYING:
      updateGame();
      drawGame();
      break;

    case GAME_OVER:
      drawGameOver();
      if (pressed) {
        score = 0;
        resetBall();
        state = PLAYING;
      }
      break;
  }

  delay(30);
}
