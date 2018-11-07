#include "LedControl.h"


//LED CONTROL
int dinPin = 12;
int clkPin = 10;
int csPin = 11;
int ledPanels = 1;
LedControl lc = LedControl(dinPin, clkPin, csPin, ledPanels);

//BUTTON PINS
int lButPin = 2;
int rButPin = 3;

/////////////////////
//   GAME BOARD   //
///////////////////


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

void rain() {
  if (board[0] == 0) {
    board[0] = rngByte();
    return;
  }
  else {
    for (int i = 5; i >= 0; i--) {
      board[i + 1] = board[i];
      delay(100);
      board[i] = B00000000;
    }
    return;
  }
}

void draw() {
  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, board[i]);
  }
}

byte rngByte() {
  byte aByte = random(0, 255) & random(0, 255) & random(0, 255);
  return aByte;
}

/////////////////////////
//   INPUT HANDLING   //
///////////////////////

/*
  void lShift() {
  if (debounce() == true && board[7] != 128) {
    board[7]

  }
  }
*/


//CODE IN setup() GETS RUN ONCE
void setup() {
  //Serial communication
  Serial.begin(9600);

  //RANDOM
  randomSeed(analogRead(A0));

  /*
    //Interrupts
    pinMode(lButPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(lButPin), lShift, RISING);
    pinMode(rButPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(rButPin), rShift, RISING);
  */
  //Turn on all attached displays
  for (int i = 0; i < lc.getDeviceCount(); i++) {
    lc.shutdown(i, false);
    lc.setIntensity(i, 1);
    lc.clearDisplay(i);
  }
}

//CODE IN loop() GETS RUN REPEATEDLY
void loop() {
  interrupts();
  rain();
  draw();
  noInterrupts();
  //check();

}
