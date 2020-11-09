/*
 * Ping Pong Rally Counter - Tim Russell
 */

#include "LedControl.h"

// ** DOT MATRIX DISPLAY **
LedControl lc = LedControl(10,8,9,1);
const byte NUMBERS[10][8] = { // data for displaying numbers, e.g. NUMBERS[3] = byte data for '3'
  {B00000000,B00111100,B01100110,B01101110,B01110110,B01100110,B01100110,B00111100}, // 0...
  {B00000000,B00011000,B00011000,B00111000,B00011000,B00011000,B00011000,B01111110},
  {B00000000,B00111100,B01100110,B00000110,B00001100,B00110000,B01100000,B01111110},
  {B00000000,B00111100,B01100110,B00000110,B00011100,B00000110,B01100110,B00111100},
  {B00000000,B00001100,B00011100,B00101100,B01001100,B01111110,B00001100,B00001100},
  {B00000000,B01111110,B01100000,B01111100,B00000110,B00000110,B01100110,B00111100},
  {B00000000,B00111100,B01100110,B01100000,B01111100,B01100110,B01100110,B00111100},
  {B00000000,B01111110,B01100110,B00001100,B00001100,B00011000,B00011000,B00011000},
  {B00000000,B00111100,B01100110,B01100110,B00111100,B01100110,B01100110,B00111100},
  {B00000000,B00111100,B01100110,B01100110,B00111110,B00000110,B01100110,B00111100}  // ...9
};
const byte TEN_COUNT[9] = { // individual 'dots' representing tens
  B00000000,B00000001,B00000011,B00000111,B00001111,B00011111,B00111111,B01111111,B11111111
};

// ** SOUND SENSOR **
const byte SOUND_PIN = 3; // arduino input pin accepting the sound sensor's digital output
int digitalSoundValue; // stores the digital value coming from the sound sensor

// Counters for the rally counting logic
byte hitCounter = 0;
byte tenCounter = 0;

// Hit detection delay
const int DETECTION_DELAY = 400; // ** Change delay between detections here (ms) **
unsigned long lastDetection = 0;

// Game timeout
const int TIMEOUT_LENGTH = 2500; // ** Change timeout length here (ms) **
unsigned long timeoutStartTime = 0;
bool timeoutStart = false;

void setup() {
  lc.shutdown(0, false); // the MAX72XX is in power-saving mode on startup, so do a wakeup call,
  lc.setIntensity(0, 8); // set the brightness to a medium value,
  lc.clearDisplay(0);    // and clear the display
  pinMode(SOUND_PIN, INPUT); // accept digital input from sound pin
  writeMatrixRows(NUMBERS[hitCounter]);
}

void loop() {
  digitalSoundValue = digitalRead(SOUND_PIN);
  
  // If sound detected & enough time passed since last detection:
  if (digitalSoundValue == HIGH && (millis() >= lastDetection + DETECTION_DELAY)) {
    if (hitCounter == 9) { // increase counters
      tenCounter++;
      hitCounter = 0;
    } else {
      hitCounter++;
    }
    writeMatrixRows(NUMBERS[hitCounter]); // display new value
    lastDetection = millis(); // update detection
    timeoutStartTime = millis(); // and timeout start times
    timeoutStart = true; // and start timeout
    
  // If timeout length reached since last sound:
  } else if (timeoutStart && (millis() >= timeoutStartTime + TIMEOUT_LENGTH)) {
      flash(3, 250); // flash screen
      hitCounter = 0; // reset counters etc.
      tenCounter = 0;
      timeoutStart = false;
      writeMatrixRows(NUMBERS[hitCounter]); // display 0
  }
  // Otherwise, just keep displaying the score
//  writeMatrixRows(NUMBERS[hitCounter]);
}

/** Writes an array of bytes, each representing a row, to the 8x8 dot matrix display */
void writeMatrixRows(byte byteData[]) {
  if (tenCounter > 8) { // limit tenCounter since we only have 8 dots!
    tenCounter = 8;
  }
  byteData[0] = TEN_COUNT[tenCounter];
  for (byte row = 0; row < 8; row++) {
    lc.setRow(0, row, byteData[row]);
  }
}

/** Flash the screen n times, with delayLen ms between each state. n must be <= 255 */
void flash(byte n, int delayLen) {
  for (byte i = 0; i < n; i++) {
    lc.clearDisplay(0);
    delay(delayLen);
    writeMatrixRows(NUMBERS[hitCounter]);
    delay(delayLen);
  }
}
