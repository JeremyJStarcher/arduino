#include <EEPROM.h>
#include "common.h"
#include "crc.c"
#include "ioline.h"
#include "ioserial.h"
#include "xmodem.h"


#define READ_TIMEOUT 100

const char message[] = "This quick brown fox jumped over the lazy dogs, stretching out his legs and kicking back and forth as he did.";
size_t message_head = 0;

byte boardRole = 0;

bool is_passing = true;

void setup() {
  Serial.begin(USB_BAUD);
  Serial2.begin(UART_BAUD);
  Serial3.begin(UART_BAUD);

  while (!Serial) ; // wait for Arduino Serial Monitor to open
  Serial.println(F("\n\n\nUSB Connection established"));

  // EEPROM.update(BOARD_ROLE_ADDRESS, BOARD_ROLE_SLAVE);
  // EEPROM.update(BOARD_ROLE_ADDRESS, BOARD_ROLE_MASTER);

  boardRole = EEPROM.read(BOARD_ROLE_ADDRESS);
  if (boardRole == BOARD_ROLE_MASTER) {
    Serial.println(F("Master board.  In control."));
  } else if (boardRole == BOARD_ROLE_SLAVE) {
    Serial.println(F("Slave board"));
  } else {
    Serial.println(F("No role established for this board.  Dying."));
    while (true)
      ;
  }

  runTests();
}

void loop() {
}


void runTests() {
  IoSerial serialHardware2;
  IoSerial serialHardware3;

  serialHardware2.begin(&Serial2);
  serialHardware3.begin(&Serial3);

  Serial.println(F("Running tests..."));
  is_passing = true;

  if (is_passing) wiringTest();
  if (is_passing) ioSerialTest(serialHardware2, serialHardware3);
  if (is_passing) ioSerialFlushTest(serialHardware2, serialHardware3);

  if (!is_passing) {
    Serial.println(F("FAILED"));
  }

  Serial.println(F("Done"));
}

byte getRandom() {
  delay(100); // Give time for it to randomize
  return analogRead(0) & 0xFF;
}
void wiringTest() {
  byte phase = 0;
  long startTime;

  const byte value2 = getRandom();
  const byte value3 = getRandom();
  signed int c;

  Serial.println(F("=== Wiring Test Setup"));
  Serial2.write(value2);
  Serial3.write(value3);
  phase = 1;
  startTime = millis();

  while (phase == 1) {
    if (millis() == startTime + (1000)) {
      if (!Serial2.available()) {
        Serial.println(F("ERROR: Did not receive signal on Serial2"));
      }
      if (!Serial3.available()) {
        Serial.println(F("ERROR: Did not receive signal on Serial3"));
      }
      is_passing = false;
      return;
    }

    if (Serial2.available() && Serial3.available()) {
      phase = 2;
    }
  }

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
  Serial.println(F("=== Wiring Test Passed"));
}

void ioSerialTest(
  IoSerial serialA,
  IoSerial serialB
) {
  long startTime;
  const long wait_time = 1000;
  const byte value2 = getRandom();
  const byte value3 = getRandom();
  signed int c;

  serialA.writebyte(value2);
  serialB.writebyte(value3);

  Serial.println(F("=== IoSerial Test Setup"));
  startTime = millis();

  c = serialA.readbyte(READ_TIMEOUT);
  if (c < 0) {
    Serial.println(F("ERROR: Did not receive signal on serialA"));
    is_passing = false;
    return;
  }

  if (c == value3) {
    Serial.print(F("serialA ** PASSED: "));
    Serial.println(c, HEX);
  } else {
    is_passing = false;
    Serial.print(F("serialA received wrong value: "));
    Serial.println(c, HEX);
    return;
  }

  c = serialB.readbyte(READ_TIMEOUT);
  if (c < 0) {
    Serial.println(F("ERROR: Did not receive signal on serialB"));
    is_passing = false;
    return;
  }

  if (c == value2) {
    Serial.print(F("serialB ** PASSED: "));
    Serial.println(c, HEX);
  } else {
    is_passing = false;
    Serial.print(F("serialB received wrong value: "));
    Serial.println(c, HEX);
    return;
  }
  Serial.println(F("=== IoSerial Test Passed"));
}

void ioSerialFlushTest(
  IoSerial serialA,
  IoSerial serialB
) {
  Serial.println(F("=== IoSerialFlush Test Setup"));
  for (int i = 0; i < 10; i++) {
    const byte value2 = getRandom();
    const byte value3 = getRandom();

    serialA.writebyte(value2);
    serialB.writebyte(value3);
  }

  serialA.flush();
  serialB.flush();

  signed int c1 = serialA.readbyte(READ_TIMEOUT);
  if (c1 >= 0) {
    Serial.println(F("ERROR: serialA did not flush properly"));
    is_passing = false;
    return;
  }
  Serial.println(F("serialA ** PASSED: "));

  signed int c2 = serialB.readbyte(READ_TIMEOUT);
  if (c2 >= 0) {
    Serial.println(F("ERROR: serialB did not flush properly"));
    is_passing = false;
    return;
  }
  Serial.println(F("serialB ** PASSED: "));
  Serial.println(F("=== IoSerialFlush Test Passed"));
}
