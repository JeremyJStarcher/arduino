//#define RUN_FS_TEST

void redrawScreen(void);

#include <SPI.h>
#include <OSFS.h>
#include <extEEPROM.h>    //https://github.com/PaoloP74/extEEPROM
#include <Keyboard.h>

#include "Adafruit_Keypad.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735

const int debounceTime = 20; // number of milliseconds for switch to be stable

#include "hardware-config.h"
// #include "eeprom-test.h"

#include "keyboard.h"

#include "data.h"

void redrawScreen(void) {
  tft.setCursor(0, 0);
  tft.setTextWrap(false);

  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(1);
  for (size_t i = 0; i < strlen(allKeys); i++) {
    char key = allKeys[i];
    char *fname = getNameFile(key);
    char *n = readFile(fname, "");

    tft.setTextColor(ST77XX_GREEN);
    tft.print(key);
    tft.setTextColor(ST77XX_WHITE);
    tft.print(" ");
    tft.println(n);
  }
}

void setup(void) {
  Serial.begin(9600);
  //  while (!Serial) {
  //    ;
  //  }

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

  keyboard_begin();
  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB); // Init ST7735S chip, black tab
  redrawScreen();
}

void loop() {
  data_loop();

  char key = getKey();
  if ( key != 0) { // if the character is not 0 then it's a valid key press
    Serial.print(F("Got key! "));
    Serial.println(key);
    char *fname = getTextFile(key);
    char *n = readFile(fname, "");
    sendString(n);

    Serial.println(n);

    tft.invertDisplay(true);
    delay(100);
    tft.invertDisplay(false);
  }

  delay(10);
}

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


size_t str_i = 0;
static const char B_ENTER[] PROGMEM = "{ENTER}";

const char * const codes[] PROGMEM = {B_ENTER};

bool prefix(const char pre[] PROGMEM, const char *str) {
  const char *s = &str[0] + str_i;
  return strncmp_P(s, pre, strlen_P(pre)) == 0;
}

void sendBrace(const char *s) {
  if (prefix(codes[0], s)) {
    Serial.println("***ENTER***");
  }
}

void sendString(const char *s) {
 // char *buffer = pgm_read_word((B_ENTER));
 // Serial.print("TEST: ");
 // Serial.println(buffer);

  str_i = 0;
  while (str_i < strlen(s)) {
    char ch = s[str_i];
    if (ch == '{') {
      Serial.println("--FOUND BRACE --");
      sendBrace(s);
      str_i += 1;
      continue;
    }
    Keyboard.press(ch);
    Keyboard.releaseAll();
    str_i += 1;
  }
}
