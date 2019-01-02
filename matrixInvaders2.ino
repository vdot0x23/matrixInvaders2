#include "LedControl.h"

//RESET GAME
void (*resetGame) (void) = 0;

//LED CONTROL
int dinPin = 12;
int clkPin = 10;
int csPin = 11;
int ledPanels = 1;

LedControl lc = LedControl(dinPin, clkPin, csPin, ledPanels);

/////////////////////
//   GAME BOARD   //
///////////////////

byte rainDrops[] = {
  B00000001,
  B00000010,
  B00000100,
  B00001000,
  B00010000,
  B00100000,
  B01000000,
  B10000000,
};

byte maxByte = B11111111;
byte player = B00010000;

byte board[] = {
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  player ^ maxByte,
};

byte lastRandom = B00010000;

void genDrops() {
  board[0] = rainDrops[random(0, 8)];
  if ((lastRandom >> 1) == board[0] || (lastRandom << 1) == board[0] || lastRandom == board[0]) {
    genDrops();
  }
  lastRandom = board[0];
}

float rainRate = 500;
unsigned long long rainRateTimer = 0;

void rain() {
  if (rainRate < millis() - rainRateTimer) {
    if (board[0] == 0) {
      rainRateTimer = millis();
      genDrops();
    }
    else {
      for (int i = 5; i >= 0; i--) {
        board[i + 1] = board[i];
        board[i] = B00000000;
      }
    }
  }
}

int difficultyRate = 100;
unsigned long long difficultyRateTimer = 0;

float initDifficulty(int x) {
  float a = rainRate;
  for (int i = 0; i < x; i++) {
    a *= 0.999;
  }
  return (a);
}

void difficulty() {
  if (difficultyRate < millis() - difficultyRateTimer) {
    difficultyRateTimer = millis();
    rainRate *= 0.999;
  }
}

void draw(byte a[]) {
  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, a[i]);
  }
}

void check() {
  byte emptyScreen[] = {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
  };

  if ((board[7] | board[6]) == 255) {
    for (int i = 0; i < 3; i++) {
      draw(board);
      delay(300);
      draw(emptyScreen);
      delay(300);
    }
    Serial.print("Your score was: ");
    Serial.println(500 - rainRate);
    delay(100);
    resetGame();
  }
}

/////////////////////////
//   INPUT HANDLING   //
///////////////////////

int lButPin = 2;
int rButPin = 3;

int inputRate = 125;
unsigned long long inputRateTimer = 0;

void shiftL() {
  if (player < 65 && inputRate < millis() - inputRateTimer) {
    inputRateTimer = millis();
    player = player << 1;
    board[7] = player ^ maxByte;
  }
}

void shiftR() {
  if (player > 1 && inputRate < millis() - inputRateTimer) {
    inputRateTimer = millis();
    player = player >> 1;
    board[7] = player ^ maxByte;
  }
}

void inputCheck() {
  if (digitalRead(lButPin) == LOW) {
    shiftL();
  }
  else if (digitalRead(rButPin) == LOW) {
    shiftR();
  }
}

////////////////////
//   START MENU  //
//////////////////

float startMenuTime = 5000;
unsigned long long menuTimer = 0;

int startInput(int dSelect) {
  if (startMenuTime < millis() - menuTimer) {
    menuTimer = millis();
    dSelect = 4;
  }
  else if (digitalRead(lButPin) == LOW && dSelect > 1 && inputRate < millis() - inputRateTimer) {
    inputRateTimer = millis();
    dSelect--;
  }
  else if (digitalRead(rButPin) == LOW && dSelect < 3 && inputRate < millis() - inputRateTimer) {
    inputRateTimer = millis();
    dSelect++;
  }
  return (dSelect);
}

void start() {

  byte startScreen1[] = {
    B00000000,
    B00111000,
    B01011000,
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00111100,
  };

  byte startScreen2[] = {
    B00000000,
    B00111000,
    B01000100,
    B00000100,
    B00001000,
    B00010000,
    B00100000,
    B01111110,
  };

  byte startScreen3[] = {
    B00000000,
    B01111000,
    B00000100,
    B00000100,
    B01111000,
    B00000100,
    B00000100,
    B01111000,
  };

  struct level {
    int easy;
    int medium;
    int hard;
  } level1;

  level1.easy = 0;
  level1.medium = 300;
  level1.hard = 600;

  int dSelect = 1;
  float tempRainRate = 0;

  while (dSelect < 4) {
    switch (dSelect) {
      case 1:
        draw(startScreen1);
        tempRainRate = initDifficulty(level1.easy);
        break;

      case 2:
        draw(startScreen2);
        tempRainRate = initDifficulty(level1.medium);
        break;

      case 3:
        draw(startScreen3);
        tempRainRate = initDifficulty(level1.hard);
        break;
    }
    dSelect = startInput(dSelect);
  }
  rainRate = tempRainRate;
}

//CODE IN setup() GETS RUN ONCE
void setup() {
  //Serial communication
  Serial.begin(9600);

  randomSeed(analogRead(A0));

  //Input
  pinMode(lButPin, INPUT_PULLUP);
  pinMode(rButPin, INPUT_PULLUP);

  //Turn on all attached displays
  for (int i = 0; i < lc.getDeviceCount(); i++) {
    lc.shutdown(i, false);
    lc.setIntensity(i, 1);
    lc.clearDisplay(i);
  }
  start();
}

//CODE IN loop() GETS RUN REPEATEDLY
void loop() {
  rain();
  inputCheck();
  draw(board);
  check();
  difficulty();
}
