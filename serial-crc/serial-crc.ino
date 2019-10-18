#include "common.h"
#include "crc.c"

#include "ioline.h"
#include "ioserial.h"

const char message[] = "This quick brown fox jumped over the lazy dogs, stretching out his legs and kicking back and forth as he did.";
size_t message_head = 0;

bool is_passing = true;

#define CODE_SETUP 0
#define CODE_WIRING_TEST 1

#define CODE_DONE 900
#define CODE_FAILED 999

uint16_t testCode = CODE_SETUP;

void run_test(void (*test)());

IoSerial IoSerial2;
IoSerial IoSerial3;

void setup() {
  Serial.begin(USB_BAUD);

  Serial2.begin(UART_BAUD);
  Serial3.begin(UART_BAUD);

  while (!Serial) ; // wait for Arduino Serial Monitor to open
  Serial.println(F("USB Connection established"));

  IoSerial2.begin(&Serial2);
  IoSerial3.begin(&Serial3);
}

void loop() {
  runTests();
}

void run_test(void (*test)())
{
  test();
}

#define ShowOnce(x) if (testCode != zzz)  {Serial.print(x);}

void runTests() {
  bool is_done;

  static uint16_t nextTestCode = -1;
  static uint16_t lastTestCode = -1;
  static uint16_t testCodeNext = CODE_SETUP;
  static uint16_t testBytesNeeded;
  static uint16_t zzz;

  zzz = lastTestCode;
  lastTestCode = testCode;

  if (is_passing == false && testCode != CODE_DONE) {
    testCode = CODE_FAILED;
  }

  switch (testCode) {
    case CODE_SETUP:
      Serial.println(F("Running tests..."));
      is_passing = true;
      testCode = CODE_WIRING_TEST;
      break;
    case CODE_WIRING_TEST:
      if (wiringTest(testCode)) {
        testCode = CODE_DONE;
      }
      break;
    case CODE_DONE:
      ShowOnce(F("Done"));
      break;
    case CODE_FAILED:
      Serial.println(F("FAILED"));
      testCode = CODE_DONE;
      break;
  }
}

bool wiringTest(uint16_t testCode) {
  static byte phase = 0;
  static long startTime;

  const byte value2 = 0x22;
  const byte value3 = 0x33;
  int c;

  if (phase == 0) {
    Serial.println(F("=== Wiring Test Setup"));
    Serial2.write(value2);
    Serial3.write(value3);
    phase = 1;
    startTime = millis();
    return false;
  }

  while (phase == 1) {
    if (millis() == startTime + (1000)) {
      if (!Serial2.available()) {
        Serial.println(F("ERROR: Did not receive signal on Serial2"));
      }
      if (!Serial3.available()) {
        Serial.println(F("ERROR: Did not receive signal on Serial3"));
      }
      is_passing = false;
      return false;
    }

    if (Serial2.available() && Serial3.available()) {
      phase = 2;
    }
    return false;
  }

  if (phase == 2) {
    c = Serial2.read();
    if (c == value3) {
      Serial.print(F("Serial2 ** PASSED: "));
      Serial.println(c, HEX);
    } else {
      is_passing = false;
      Serial.print(F("Serial2 received wrong value: "));
      Serial.println(c, HEX);
    }

    c = Serial3.read();
    if (c == value2) {
      Serial.print(F("Serial3 ** PASSED: "));
      Serial.println(c, HEX);
    } else {
      is_passing = false;
      Serial.print(F("Serial3 received wrong value: "));
      Serial.println(c, HEX);
    }
    return true;
  }
}
