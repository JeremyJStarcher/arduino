#include <EEPROM.h>
#include <avr/pgmspace.h>
#include "common.h"
#include "ioline.h"
#include "ioserial.h"
#include "xmodem.h"


/* Wiring between two MEGAs

    From    To
    M14     M17
    M15     M16
    S14     S17
    S15     S16
    M18     S19
    M19     S18
    M8      S<reset>
    M<gnd>  S<gnd>

    For Master:
    A2      HIGH

    For Slave
    A2      LOW
*/

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define CTRLZ 0x1A

#define READ_TIMEOUT 100

const char poolStr[] = "ABCEDFGHIJKLMNOPQRSTUVWXYZ012345670";
const size_t longMessageLen = 2048;
unsigned char longMessage[longMessageLen];

const char shortMessage[] PROGMEM = {'M', 'S', 'G', 0, '1', '7', '0', '1'};
const byte shortMessageLength = 8;

bool isBoardMaster = false;

bool is_passing = true;
const int slaveResetPin = 8;
const int masterSelectPin = A2;

void setup() {
  Serial.begin(USB_BAUD);
  Serial1.begin(UART_BAUD);
  Serial2.begin(UART_BAUD);
  Serial3.begin(UART_BAUD);

  while (!Serial) ; // wait for Arduino Serial Monitor to open
  Serial.println(F("\n\n\nUSB Connection established"));

  for (int i = 0; i < longMessageLen; i++) {
    longMessage[i] = poolStr[i % strlen(poolStr)];
  }

  pinMode(masterSelectPin, INPUT);

  isBoardMaster = digitalRead(masterSelectPin);

  if (isBoardMaster) {
    Serial.println(F("Master board.  In control."));
    Serial.println(F("Resetting slave"));
    pinMode(slaveResetPin, OUTPUT);
    digitalWrite(slaveResetPin, LOW);
    delay(1000);
    digitalWrite(slaveResetPin, HIGH);
    pinMode(slaveResetPin, INPUT);
  } else {
    Serial.println(F("Slave board"));
  }

  runTests();
}

void loop() {
}

void waitForSync() {
  delay(2 * 1000);
}

void runTests() {

  IoSerial serialHardware0;
  IoSerial serialHardware1;
  IoSerial serialHardware2;
  IoSerial serialHardware3;

  serialHardware0.begin(&Serial);
  serialHardware1.begin(&Serial1);
  serialHardware2.begin(&Serial2);
  serialHardware3.begin(&Serial3);

  Serial.println(F("Running tests..."));
  is_passing = true;

  if (is_passing) wiringTest();
  if (is_passing) ioSerialTest(serialHardware2, serialHardware3);
  if (is_passing) ioSerialFlushTest(serialHardware2, serialHardware3);
  if (is_passing) ioSerialPushTest(serialHardware2) ;

  if (is_passing && isBoardMaster) {
    waitForSync();
    receiveShortMessage(serialHardware1);
    waitForSync();
    sendShortMessage(serialHardware1);
    waitForSync();
    sendXmodem(serialHardware1);
  }

  if (is_passing && !isBoardMaster) {
    waitForSync();
    sendShortMessage(serialHardware1);
    waitForSync();
    receiveShortMessage(serialHardware1);
    waitForSync();
    receiveXmodem(serialHardware1);
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
  Serial.println("<----");

  Serial.println(F("Waiting for ACK"));
  while ((c = remote.readbyte(READ_TIMEOUT)) != ACK)
    ;
  remote.writebyte(EOT);

  Serial.println(F("=== Sending short message ** PASS"));
  remote.flush();
}

void receiveShortMessage(IoSerial remote) {
  Serial.println(F("=== Receiving short message"));
  long now = millis();
  size_t idx = 0;
  signed int c;

  Serial.println(F("Waiting for SOH"));
  // Wait for the datastream to start
  while ((c = remote.readbyte(READ_TIMEOUT)) != SOH)
    ;
  Serial.println(F("Found SOH"));

  while (true) {
    c = remote.readbyte(READ_TIMEOUT);
    Serial.println(c);

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
  Serial.println(F("Waiting for reply to our ACK"));
  while ((c = remote.readbyte(READ_TIMEOUT)) != EOT)
    ;

  if (is_passing) {
    Serial.println(F("=== Receiving short message **PASS"));
  }
  remote.flush();
}

void sendXmodem(IoSerial remote) {
  Serial.println("Sending XModem");
  XmodemCrc xmodem;
  xmodem.begin(&remote);
  int res = xmodem.xmodemTransmit(longMessage, longMessageLen);

  Serial.print("Used CRC? ");
  Serial.println(xmodem.usedCrc());
  Serial.print("Result: ");
  Serial.println(res);
}

void receiveXmodem(IoSerial remote) {
  Serial.println("Recieve XModem");
  XmodemCrc xmodem;
  xmodem.begin(&remote);
  int res = xmodem.xmodemReceive(longMessage, longMessageLen);

  Serial.print("Used CRC? ");
  Serial.println(xmodem.usedCrc());
  Serial.print("Result: ");
  Serial.println(res);
}
