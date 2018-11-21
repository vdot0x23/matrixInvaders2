#include "LedControl.h"

//LED CONTROL
int dinPin = 12;
int clkPin = 10;
int csPin = 11;
int ledPanels = 1;
LedControl lc = LedControl(dinPin, clkPin, csPin, ledPanels);

/////////////////////
//   GAME BOARD   //
///////////////////

//Generer 1 og bitshift randomly
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

float rainRate = 250;
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

void draw() {
  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, board[i]);
  }
}

void check() {
  if ((board[7] | board[6]) == 255) {
    //Serial.println("you loose");
  }
}

int difficultyRate = 100;
unsigned long long difficultyRateTimer = 0;

void difficulty() {
  if (difficultyRate < millis() - difficultyRateTimer) {
    difficultyRateTimer = millis();
    rainRate *= 0.999;
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
}

//CODE IN loop() GETS RUN REPEATEDLY
void loop() {
  rain();
  inputCheck();
  draw();
  check();
  difficulty();
}
