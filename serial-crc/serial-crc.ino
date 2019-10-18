#include "common.h"
#include "crc.c"

#include "ioline.h"
#include "ioserial.h"

const char message[] = "This quick brown fox jumped over the lazy dogs, stretching out his legs and kicking back and forth as he did.";
size_t message_head = 0;

bool is_passing = true;

#define CODE_SETUP 0
#define CODE_WIRING_SETUP 1
#define CODE_WIRING_WAIT2 2
#define CODE_WIRING_WAIT3 3

#define CODE_WIRING_EVAL 4

#define CODE_DONE 900
#define CODE_FAILED 999

#define CODE_WAITING_COM2 1000
#define CODE_WAITING_COM3 1001

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
  byte avail;
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
    case CODE_WAITING_COM2:
    case CODE_WAITING_COM3:
      ShowOnce("Waiting on serial port #");
      ShowOnce(testCode == CODE_WAITING_COM2 ? "2" : "3");
      Serial.print(F("."));


      avail = testCode == CODE_WAITING_COM2
              ? Serial2.available()
              : Serial3.available();

      if (avail >= testBytesNeeded) {
        testCode = testCodeNext;
        Serial.println(F("!"));
      }

      break;
    case CODE_SETUP:
      Serial.println(F("Running tests..."));
      is_passing = true;
      testCode = CODE_WIRING_SETUP;
      break;
    case CODE_WIRING_SETUP:
      wiringTest(testCode);
      testCode = CODE_WIRING_WAIT2;
      break;
    case CODE_WIRING_WAIT2:
      testCodeNext = CODE_WIRING_WAIT3;
      testCode = CODE_WAITING_COM2;
      testBytesNeeded = 1;
      break;
    case CODE_WIRING_WAIT3:
      testCodeNext = CODE_WIRING_EVAL;
      testCode = CODE_WAITING_COM3;
      testBytesNeeded = 1;
      break;
    case CODE_WIRING_EVAL:
      wiringTest(testCode);
      testCode = CODE_DONE;
      break;
    case CODE_DONE:
      // Serial.println(F("Done"));
      break;
    case CODE_FAILED:
      Serial.println(F("FAILED"));
      testCode = CODE_DONE;
      break;
  }
}

void wiringTest(uint16_t testCode) {
  const byte value2 = 0x22;
  const byte value3 = 0x33;
  byte c;

  if (testCode == CODE_WIRING_SETUP) {
    Serial.println(F("=== Wiring Test Setup"));
    Serial2.write(value2);
    Serial3.write(value3);
  }

  if (testCode == CODE_WIRING_EVAL) {
    if (Serial2.available()) {
      c = Serial2.read();
      if (c == value3) {
        Serial.print(F("Serial2 ** PASSED: "));
        Serial.println(c, HEX);
      } else {
        is_passing = false;
        Serial.print(F("Serial2 received wrong value: "));
        Serial.println(c, HEX);
      }
    } else {
      is_passing = false;
      Serial.println(F("Serial2 received did not receive a value: "));
    }

    if (Serial3.available()) {
      c = Serial3.read();
      if (c == value2) {
        Serial.print(F("Serial3 ** PASSED: "));
        Serial.println(c, HEX);
      } else {
        is_passing = false;
        Serial.print(F("Serial3 received wrong value: "));
        Serial.println(c, HEX);
      }
    } else {
      is_passing = false;
      Serial.println(F("Serial3 received did not receive a value: "));
    }
  }
}
