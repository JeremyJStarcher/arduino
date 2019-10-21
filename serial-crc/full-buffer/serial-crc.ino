#include <avr/pgmspace.h>
#include "common.h"
#include "ioline.h"
#include "ioserial.h"
#include "xmodem-old.h"
#include "xmodem-crc.h"

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

const char poolStr[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670";
const size_t longMessageLen = XMODEM_BLOCKSIZE * 2;
const size_t longMessageBufferLen = XMODEM_BLOCKSIZE * 3;

unsigned char longMessage[longMessageBufferLen];

const char shortMessage[] PROGMEM = {'M', 'S', 'G', 0, '1', '7', '0', '1'};
const byte shortMessageLength = 8;

bool isBoardMaster = false;

bool is_passing = true;
const int slaveResetPin = 8;
const int masterSelectPin = A2;

void populateLongMessage() {
  for (int i = 0; i < longMessageBufferLen; i++) {
    longMessage[i] = poolStr[i % strlen(poolStr)];
  }
}

void setup() {
  Serial.begin(USB_BAUD);
  Serial1.begin(UART_BAUD);
  Serial2.begin(UART_BAUD);
  Serial3.begin(UART_BAUD);

  while (!Serial) ; // wait for Arduino Serial Monitor to open
  Serial.println(F("\n\n\nUSB Connection established"));

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

  runOldTests();
  runXModemTests();

  if (is_passing) {
    blinkSuccess();
  } else {
    blinkError();
  }
}

void loop() {
}

void waitForSync() {
  delay(2 * 1000);
}

void blinkError() {
  pinMode(LED_BUILTIN, OUTPUT);

  const int blinkRate = 100;
  while (true) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(blinkRate);
    digitalWrite(LED_BUILTIN, LOW);
    delay(blinkRate);
  }
}


void blinkSuccess() {
  pinMode(LED_BUILTIN, OUTPUT);

  const int blinkRate = 100;
  while (true) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(blinkRate * 4);
    digitalWrite(LED_BUILTIN, LOW);
    delay(blinkRate);
  }
}

void runXModemTests() {
  IoSerial serialHardware0;
  IoSerial serialHardware1;
  IoSerial serialHardware2;
  IoSerial serialHardware3;

  serialHardware0.begin(&Serial);
  serialHardware1.begin(&Serial1);
  serialHardware2.begin(&Serial2);
  serialHardware3.begin(&Serial3);

  Serial.println(F("Running tests..."));
  for (int offset = -5; offset <= 5; offset++) {
    if (isBoardMaster) {
      if (is_passing) waitForSync();
      if (is_passing) receiveShortMessage(serialHardware1);
      if (is_passing) waitForSync();
      if (is_passing) sendNewXmodem(serialHardware1, offset);
      if (is_passing) waitForSync();
      if (is_passing) receiveNewXmodem(serialHardware1, offset);
    }

    if (!isBoardMaster) {
      Serial.println("MAY NEED TO RESET MASTER");
      if (is_passing) waitForSync();
      if (is_passing) sendShortMessage(serialHardware1);
      if (is_passing) waitForSync();
      if (is_passing) receiveOldXmodem(serialHardware1, offset);
      if (is_passing) waitForSync();
      if (is_passing) sendOldXmodem(serialHardware1, offset);
    }
  }

  if (!is_passing) {
    Serial.println(F("FAILED"));
  }

  Serial.println(F("Done"));
}

void runOldTests() {
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

  if (isBoardMaster) {
    if (is_passing) waitForSync();
    if (is_passing) receiveShortMessage(serialHardware1);
    if (is_passing) waitForSync();
    if (is_passing) sendShortMessage(serialHardware1);
    if (is_passing) waitForSync();
    if (is_passing) sendOldXmodem(serialHardware1, 0);
  }

  if (!isBoardMaster) {
    Serial.println("MAY NEED TO RESET MASTER");
    if (is_passing) waitForSync();
    if (is_passing) sendShortMessage(serialHardware1);
    if (is_passing) waitForSync();
    if (is_passing) receiveShortMessage(serialHardware1);
    if (is_passing) waitForSync();
    if (is_passing) receiveOldXmodem(serialHardware1, 0);
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
    Serial.println(F("Data was found in the buffer. Failed"));
    is_passing = false;
    return;
  }

  serial.push(0xA5);
  c1 = serial.readbyte(READ_TIMEOUT);
  if (c1 != 0xA5) {
    Serial.println(F("Pushed data not found.  Failed."));
    is_passing = false;
    return;
  }

  c1 = serial.readbyte(READ_TIMEOUT);
  if (c1 != -1) {
    Serial.println(F("Data was found in the buffer after reading pushed data. Failed"));
    is_passing = false;
    return;
  }

  serial.push(0xA5);
  serial.flush();
  c1 = serial.readbyte(READ_TIMEOUT);
  if (c1 != -1) {
    Serial.println(F("Data was found in the buffer after flushing pushed data. Failed"));
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
  Serial.println(F("<----"));

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

void fillTmp(char *tmp, size_t len, char value) {
  memset(tmp, value, len);
}

bool tmpMatch (unsigned char *tmp, unsigned char *str, size_t len, int offset) {
  Serial.print(F("*****"));
  Serial.println(len);
  size_t strLen = len + offset;
  bool ret = true;

  for (int i = 0; i < strLen; i++) {
    if (tmp[i] != str[i]) {
      Serial.print(i);
      Serial.print("\t");
      Serial.print(tmp[i]);
      Serial.print("\t");
      Serial.println(str[i]);

      Serial.println(F("String contents did not match"));
      ret = false;
      break;
    }
  }

  for (int i = strLen; i < longMessageBufferLen; i++) {
    if (tmp[i] != FILL_BYTE) {
      Serial.print(F("Fill byte overwritten at "));
      Serial.println(i);
      Serial.print(tmp[i], DEC);
      Serial.println("");

      ret = false;
      break;
    }
  }

#if 0
  for (int i = 0; i < longMessageBufferLen; i++) {
    Serial.print(tmp[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");
#endif

  return ret;
}

void sendOldXmodem(IoSerial remote, int offset) {
  Serial.print(F("Sending OLD XModem with offset "));
  Serial.println(offset);

  XmodemOld xmodem;
  xmodem.begin(&remote);
  populateLongMessage();

  size_t mesgLen = longMessageLen + offset;
  int res = xmodem.xmodemTransmit(longMessage, mesgLen);

  Serial.print(F("Used CRC? "));
  Serial.println(xmodem.usedCrc());
  Serial.print(F("Result: "));
  Serial.println(res);
}

void receiveOldXmodem(IoSerial remote, int offset) {
  Serial.print(F("receive OLD XModem with offset "));
  Serial.println(offset);

  XmodemOld xmodem;
  xmodem.begin(&remote);
  populateLongMessage();

  unsigned char tmp[longMessageBufferLen];
  fillTmp(tmp, longMessageBufferLen, FILL_BYTE);

  size_t mesgLen = longMessageLen + offset;
  int res = xmodem.xmodemReceive(tmp, mesgLen);

  if (!tmpMatch(tmp, longMessage, longMessageLen, offset)) {
    is_passing = false;
  }

  Serial.print(F("Used CRC? "));
  Serial.println(xmodem.usedCrc());
  Serial.print(F("Result: "));
  Serial.println(res);
}

void sendNewXmodem(IoSerial remote, int offset) {
  Serial.print(F("Sending new XModem with offset "));
  Serial.println(offset);

  XmodemCrc xmodem;
  populateLongMessage();

  unsigned short crc1 = crc16_ccitt(longMessage, longMessageLen);
  unsigned short crc2 = 0;
  for (int i = 0; i < longMessageLen; i++) {
    crc2 = xmodem.crc16_ccitt(crc2, longMessage[i]);
  }

  size_t mesgLen = longMessageLen + offset;
  xmodem.transmit(&remote, longMessage, mesgLen);

  while (!xmodem.isDone()) {
    Serial.print(F("(T) Packet: "));
    Serial.print(xmodem.getPacketNumber());
    Serial.print("\t");
    Serial.print(xmodem.getStatus());
    Serial.print("\t");
    Serial.print(xmodem.getState());
    Serial.println("");

    xmodem.next();
  }

  Serial.print(F("Status: "));
  Serial.println(xmodem.getStatus());

  if (xmodem.getStatus() < 0) {
    is_passing = false;
  }

  if (is_passing ) {
    Serial.println(F("Sending new XModem **PASS"));
  } else {
    Serial.println(F("Sending new XModem **FAIL"));
  }
}

void receiveNewXmodem(IoSerial remote, int offset) {
  Serial.print(F("Receive new XModem with offset "));
  Serial.println(offset);

  XmodemCrc xmodem;
  populateLongMessage();

  unsigned char tmp[longMessageBufferLen];
  fillTmp(tmp, longMessageBufferLen, FILL_BYTE);

  size_t mesgLen = longMessageLen + offset;
  xmodem.receive(&remote, tmp, mesgLen);

  while (!xmodem.isDone()) {
    Serial.print(F("(R) Packet:"));
    Serial.print(xmodem.getPacketNumber());
    Serial.print("\t");
    Serial.print(xmodem.getStatus());
    Serial.print("\t");
    Serial.print(xmodem.getState());
    Serial.println("");

    xmodem.next();
  }

  Serial.print(F("Status: "));
  Serial.println(xmodem.getStatus());

  if (!tmpMatch(tmp, longMessage, longMessageLen, offset)) {
    is_passing = false;
  }

  if (xmodem.getStatus() < 0) {
    is_passing = false;
  }

  if (is_passing) {
    Serial.println(F("Receive new XModem **PASS"));
  } else {
    Serial.println(F("Receive new XModem **FAIL"));
  }
}
