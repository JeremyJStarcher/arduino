#include <EEPROM.h>
#include <avr/pgmspace.h>
#include "common.h"
#include "crc.c"
#include "ioline.h"
#include "ioserial.h"
#include "xmodem.h"

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define CTRLZ 0x1A

#define READ_TIMEOUT 100

const char longMessage[] PROGMEM = {"This quick brown fox jumped over the lazy dogs, stretching out his legs and kicking back and forth as he did."};
const char shortMessage[] PROGMEM = {'M', 'S', 'G', 0, '1', '7', '0', '1'};
const byte shortMessageLength = 8;

byte boardRole = 0;

bool is_passing = true;

void setup() {
  Serial.begin(USB_BAUD);
  Serial1.begin(UART_BAUD);
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

  IoSerial serialHardware1;
  IoSerial serialHardware2;
  IoSerial serialHardware3;

  serialHardware1.begin(&Serial1);
  serialHardware2.begin(&Serial2);
  serialHardware3.begin(&Serial3);

  Serial.println(F("Running tests..."));
  is_passing = true;

  if (is_passing) wiringTest();
  if (is_passing) ioSerialTest(serialHardware2, serialHardware3);
  if (is_passing) ioSerialFlushTest(serialHardware2, serialHardware3);
  if (is_passing) ioSerialPushTest(serialHardware2) ;

  if (is_passing && boardRole == BOARD_ROLE_MASTER) {
    sendShortMessage(serialHardware1);
    receiveShortMessage(serialHardware1);
  }

  if (is_passing && boardRole == BOARD_ROLE_SLAVE) {
    Serial.println(F("Waiting for datastream.  Reset MASTER Arduino."));
    receiveShortMessage(serialHardware1);
    // Crappy workaround for a race condition.
    delay(2 * 1000);
    sendShortMessage(serialHardware1);
  }

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
  Serial.println(F("=== IoSerialFlushTest Setup"));
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
  Serial.println(F("=== IoSerialFlushTest Passed"));
}

void ioSerialPushTest(IoSerial serial) {
  Serial.println(F("=== ioSerialPushTest Flush Test"));
  signed int c1;

  serial.flush();
  c1 = serial.readbyte(READ_TIMEOUT);
  if (c1 != -1) {
    Serial.println("Data was found in the buffer. Failed");
    is_passing = false;
    return;
  }

  serial.push(0xA5);
  c1 = serial.readbyte(READ_TIMEOUT);
  if (c1 != 0xA5) {
    Serial.println("Pushed data not found.  Failed.");
    is_passing = false;
    return;
  }

  c1 = serial.readbyte(READ_TIMEOUT);
  if (c1 != -1) {
    Serial.println("Data was found in the buffer after reading pushed data. Failed");
    is_passing = false;
    return;
  }

  serial.push(0xA5);
  serial.flush();
  c1 = serial.readbyte(READ_TIMEOUT);
  if (c1 != -1) {
    Serial.println("Data was found in the buffer after flushing pushed data. Failed");
    is_passing = false;
    return;
  }
  Serial.println(F("=== ioSerialPushTest Test Passed"));
}

void sendShortMessage(IoSerial remote) {
  unsigned int c;

  Serial.println(F("=== Sending short message"));
  remote.writebyte(SOH);
  for (size_t i = 0; i < shortMessageLength; i++) {
    byte b = pgm_read_byte_near(shortMessage + i);
    remote.writebyte(b);
    Serial.write(b);
  }
  Serial.println("");


  Serial.print(F("Waiting for ACK"));
  while ((c = remote.readbyte(READ_TIMEOUT)) != ACK)
    ;
  remote.writebyte(EOT);

  Serial.println(F("\n=== Sending short message ** PASS"));
  remote.flush();
}

void receiveShortMessage(IoSerial remote) {
  Serial.println(F("=== Receiving short message"));
  long now = millis();
  size_t idx = 0;
  signed int c;

  // Wait for the datastream to start
  while ((c = remote.readbyte(READ_TIMEOUT)) != SOH)
    ;

  while (true) {
    c = remote.readbyte(READ_TIMEOUT);
    if (c == -1) continue;

    byte b = pgm_read_byte_near(shortMessage + idx);
    if (b != c) {
      Serial.print(F("\nRead failed.  Expected "));
      Serial.print(b);
      Serial.print(F(" got "));
      Serial.print(c);
      Serial.println("");
      is_passing = false;
      break;
    } else {
      Serial.write(c);

      idx++;
      if (idx == shortMessageLength) {
        break;
      }
    }

  }

  // Tell the remote we got it.
  remote.writebyte(ACK);

  Serial.println("");
  Serial.print(F("Waiting for reply to our ACK"));
  while ((c = remote.readbyte(READ_TIMEOUT)) != EOT)
    ;

  if (is_passing) {
    Serial.println(F("\n=== Receiving short message **PASS"));
  }
  remote.flush();
}
