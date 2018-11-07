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


void rain() {
  if (board[0] == 0) {
    board[0] = rainDrops[random(0, 8)];
    return;
  }
  else {
    for (int i = 5; i >= 0; i--) {
      board[i + 1] = board[i];
      delay(22);
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

void check() {
  if ((board[7] | board[6]) == 255) {
    //Serial.println("you loose");
    return;
  }
  else {
    return;
  }
}

/////////////////////////
//   INPUT HANDLING   //
///////////////////////

/*
 

volatile unsigned long lastDebounce = 0;

int debounce() {
  int debounceCutoff = 111;
  if ((long)millis() - lastDebounce >= debounceCutoff) {
    lastDebounce = millis();
    return 1;
  }
  else {
    return 0;
  }
}

int lButPin = 2;
int rButPin = 3;

void lShift() {
  if (player < 128 && debounce() == 1) {
    player = player << 1;
    board[7] = player ^ maxByte;
    draw();
  }
}

void rShift() {
  if (player > 1 && debounce() == 1) {
    player = player >> 1;
    board[7] = player ^ maxByte;
    draw();
  }
}

 */

//CODE IN setup() GETS RUN ONCE
void setup() {
  //Serial communication
  Serial.begin(9600);

  randomSeed(analogRead(A0));

  //Input interrupts

/*
  
  pinMode(lButPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(lButPin), lShift, FALLING);
  pinMode(rButPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(rButPin), rShift, FALLING);

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
  //noInterrupts();
  check();

}
