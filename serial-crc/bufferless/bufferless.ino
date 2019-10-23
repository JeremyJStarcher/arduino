#include <avr/pgmspace.h>
#include "common.h"
#include "ioline.h"
#include "ioserial.h"
#include "xmodem-old.h"
#include "xmodem-crc.h"

#define BLOCK_SIZE 128

/* Wiring between two MEGAs

  Purpose    From        To
  Loopback    [TX3] M14   [RX2] M17
  Loopback    [RX3] M15   [TX2] M16
  Loopback    [TX3] S14   [RX2] S17
  Loopback    [RX3] S15   [TX2] S16

  DataLink    [RX1] M18   [TX1] S19
  DataLink    [TX1] M19   [RX1] S18

  Slave RST         M8          S-RST

  Ground            M-GND       S-GND

  Config Master     M-A2        M-+5
  Config Slave      S-A2        S-GND

  Handshaking       M-7         S-6
  Handshaking       S-6         M-7
*/

#define ACK  0x06
#define SOH  0x01

#define READ_TIMEOUT 100

const char poolStr[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670";
const size_t longMessageLen = BLOCK_SIZE * 2;
const size_t longMessageBufferLen = BLOCK_SIZE * 3;

unsigned char longMessage[longMessageBufferLen];

bool isBoardMaster = false;

bool is_passing = true;
const int slaveResetPin = 8;
const int masterSelectPin = A2;
const int clearToSend = 7;
const int requestToSend = 6;

IoSerial serialRemoteLink;

#ifdef TEST_SERIAL_LOOPBACK
IoSerial serialLoopbackA;
IoSerial serialLoopbackB;
#endif

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

  pinMode(clearToSend, INPUT);
  digitalWrite(requestToSend, LOW);
  pinMode(requestToSend, OUTPUT);

  if (isBoardMaster) {
    Serial.println(F("Master board.  In control."));
    Serial.println(F("Resetting slave"));
    pinMode(slaveResetPin, OUTPUT);
    digitalWrite(slaveResetPin, LOW);
    delay(1000);
    digitalWrite(slaveResetPin, HIGH);
    pinMode(slaveResetPin, INPUT);

    Serial.println("Giving slave time to init");
    delay(6 * 1000);
  } else {
    Serial.println(F("Slave board"));
  }

  serialRemoteLink.begin(&Serial1);

#ifdef TEST_SERIAL_LOOPBACK
  serialLoopbackA.begin(&Serial2);
  serialLoopbackB.begin(&Serial3);
#endif

  waitForSync();

  initSerial(serialRemoteLink);

  waitForSync();

  runTests();

  waitForSync();

  if (is_passing) {
    blinkSuccess();
  } else {
    blinkError();
  }
}

void loop() {
}

void initSerial(IoSerial remote) {
  /*
     Arduino MEGA work around:

     The MEGA has this weird issue...

     Sometimes the serial ports will show traffic of all NILs (0x00) until there
     has been real traffic on the port.

     Some have recommended a 4.7K pull up resister on the RX line as a work around
     and I've heard there has been a lot of success with that.

     For my purposes, I'll just send traffic until the serial ports settle down.
  */

  if (isBoardMaster) {
    initSerialInner(remote, 1, 0);
  } else {
    initSerialInner(remote, 0, 1);
  }
}

void initSerialInner(IoSerial remote, int snd, int rcv) {
  if (snd) {
    unsigned int c;

    while ((c = remote.readbyte(READ_TIMEOUT)) != ACK) {
      remote.writebyte(SOH);
    }

    remote.flush();
  }

  if (rcv) {
    signed int c;

    while ((c = remote.readbyte(READ_TIMEOUT)) != SOH) {
      // Idle...
    }

    remote.writebyte(ACK);
    remote.flush();
  }
}

void waitForSync() {
  digitalWrite(requestToSend, HIGH);
  while (digitalRead(clearToSend) == LOW) {
    // idle
  }
  digitalWrite(requestToSend, LOW);
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
    waitForSync();
    digitalWrite(LED_BUILTIN, HIGH);
    delay(blinkRate * 4);
    digitalWrite(LED_BUILTIN, LOW);
    delay(blinkRate);
  }
}


void runTests() {
  Serial.println(F("Running tests..."));
  is_passing = true;

#ifdef TEST_SERIAL_LOOPBACK
  if (is_passing) wiringTest();
  if (is_passing) ioSerialTest(serialLoopbackA, serialLoopbackB);
  if (is_passing) ioSerialFlushTest(serialLoopbackA, serialLoopbackB);
  if (is_passing) ioSerialPushTest(serialLoopbackA) ;
#endif

  if (is_passing) waitForSync();
  if (is_passing) TestOldtoOld();

  for (int offset = -5; offset <= 5; offset++) {
    // if (is_passing) sendNewToOld(offset);
    // if (is_passing) sendOldToNew(offset);
    // if (is_passing) sendNewToNew(offset);
  }

  if (is_passing) sendNewToNew(0);

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

void fillTmp(char *tmp, size_t len, char value) {
  memset(tmp, value, len);
}

bool tmpMatch (unsigned char *tmp, unsigned char *str, size_t len, int offset) {
#if DEBUG
  Serial.print(F("*****"));
  Serial.println(len);
#endif

  size_t strLen = len + offset;
  size_t errorPosition;
  bool ret = true;

  for (int i = 0; i < strLen; i++) {
    if (tmp[i] != str[i]) {
      errorPosition = i;
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
  for (int i = 0; i < strlen; i++) {
    Serial.print(i == errorPosition ? "[" : "");
    Serial.print((char) tmp[i], HEX);
    Serial.print(i == errorPosition ? "]" : "");
    Serial.print(" ");
  }
  Serial.println("");

  for (int i = 0; i < strlen; i++) {
    Serial.print(i == errorPosition ? "[" : "");
    Serial.print((char) str[i], HEX);
    Serial.print(i == errorPosition ? "]" : "");
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

  int lastPacket = 1;
  size_t mesgLen = longMessageLen + offset;
  size_t ctr = 0;
  xmodem.transmit(&remote, BLOCK_SIZE);

  while (!xmodem.isDone()) {
    size_t startPos = ctr * BLOCK_SIZE;
    size_t bytesleft = mesgLen - startPos;

#if DEBUG
    Serial.print(F("(T) Packet: "));
    Serial.print(xmodem.getPacketNumber());
    Serial.print("\t");
    Serial.print(xmodem.getStatus());
    Serial.print("\t");
    Serial.print(xmodem.getState());
    Serial.print("\tSP\t");
    Serial.print(startPos);
    Serial.print("\tBL:\t");
    Serial.println(bytesleft);
#endif

    xmodem.nextTransmit(&longMessage[startPos], bytesleft);
    if (lastPacket != xmodem.getPacketNumber()) {
      ctr += 1;
    }
    lastPacket = xmodem.getPacketNumber();
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
#if DEBUG
  Serial.print("mesgLen = ");  Serial.println(mesgLen);
#endif

  size_t ctr = 0;
  int lastPacket = 1;

  xmodem.receive(&remote, BLOCK_SIZE);

  while (!xmodem.isDone()) {
    size_t startPos = ctr * BLOCK_SIZE;
    size_t bytesleft = mesgLen - startPos;

#if DEBUG
    Serial.print(F("(R) Packet:"));
    Serial.print(xmodem.getPacketNumber());
    Serial.print("\t");
    Serial.print(xmodem.getStatus());
    Serial.print("\t");
    Serial.print(xmodem.getState());
    Serial.print("\tSP\t");
    Serial.print(startPos);
    Serial.print("\tBL:\t");
    Serial.println(bytesleft);
#endif

    xmodem.nextRecieve(&tmp[startPos], min(BLOCK_SIZE, bytesleft));
    if (lastPacket != xmodem.getPacketNumber()) {
      ctr += 1;
    }
    lastPacket = xmodem.getPacketNumber();
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

void TestOldtoOld() {
  if (isBoardMaster) {
    sendOldXmodem(serialHardware1, 0);
  }

  if (!isBoardMaster) {
    receiveOldXmodem(serialHardware1, 0);
  }
}

void sendNewToOld(int offset) {
  if (isBoardMaster) {
    sendNewXmodem(serialHardware1, offset);
  }
  if (!isBoardMaster) {
    receiveOldXmodem(serialHardware1, offset);
  }
}

void sendOldToNew(int offset) {
  if (isBoardMaster) {
    receiveNewXmodem(serialHardware1, offset);
  }
  if (!isBoardMaster) {
    sendOldXmodem(serialHardware1, offset);
  }
}

void sendNewToNew(int offset) {
  if (isBoardMaster) {
    sendNewXmodem(serialHardware1, offset);
  }

  if (!isBoardMaster) {
    receiveNewXmodem(serialHardware1, offset);
  }
}
