#define PROGRAM_MODE
//#define RUN_FS_TEST

#include <SPI.h>
#include <OSFS.h>
#include <extEEPROM.h>    //https://github.com/PaoloP74/extEEPROM

#ifndef PROGRAM_MODE
#include "Adafruit_Keypad.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#endif

#ifdef PROGRAM_MODE
#include "data.h"
#endif

const int debounceTime = 20; // number of milliseconds for switch to be stable



#include "hardware-config.h"
#include "eeprom-test.h"
#include "keyboard.h"

void setup(void) {
  Serial.begin(9600);
#ifdef PROGRAM_MODE
  while (!Serial) {
    ;
  }
  Serial.println(F("PROGRAMMING MODE"));
#else
  Serial.println(F("KEYPAD MODE"));
#endif

  Serial.print(F("FREE MEM: "));
  Serial.println(freeMemory());

  uint8_t eepStatus = eep.begin(eep.twiClock400kHz);   //go fast!
  if (eepStatus) {
    Serial.print(F("extEEPROM.begin() failed, status = "));
    Serial.println(eepStatus);
    while (1);
  }

  Serial.println(F("EEPROM INIT SUCCESSFUL"));

#ifdef RUN_FS_TEST
  writeTest();
  readTest();
#endif

#ifndef PROGRAM_MODE
  keyboard_begin();
  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB); // Init ST7735S chip, black tab
  tft.fillScreen(ST77XX_BLACK);
#endif
}

#ifdef PROGRAM_MODE
void loop() {
  data_loop();
}
#endif


#ifndef PROGRAM_MODE
void loop() {

  char key = getKey();
  if ( key != 0) { // if the character is not 0 then it's a valid key press
    Serial.print("Got key! ");
    Serial.println(key);
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    tft.setTextColor(ST77XX_RED);
    tft.setTextSize(2);
    tft.println(key);
    tft.println("    ");
  }

  delay(10);

  // tft.invertDisplay(true);
  // delay(500);
  // tft.invertDisplay(false);
  // delay(500);
}
#endif


#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}
