#define BUFFER_SIZE 128
static unsigned char buffer[BUFFER_SIZE];

#include "./util.h"
#include "xmodem.h"

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

bool isPassing;
void setup()
{
  Serial.begin(USB_BAUD);
  Serial1.begin(SERIAL_BAUD);

  //  while (!Serial)
  //    ; // wait for Arduino Serial Monitor to open

  Serial.println(F("\n\n\nUSB Connection established"));

  pinMode(masterSelectPin, INPUT);
  isBoardMaster = digitalRead(masterSelectPin);

  pinMode(clearToSendPin, INPUT);
  digitalWrite(requestToSendPin, LOW);
  pinMode(requestToSendPin, OUTPUT);

  if (isBoardMaster)
  {
    Serial.println(F("Master board.  In control."));
    Serial.println(F("Resetting slave"));
    pinMode(slaveResetPin, OUTPUT);
    digitalWrite(slaveResetPin, LOW);
    delay(1000);
    digitalWrite(slaveResetPin, HIGH);
    pinMode(slaveResetPin, INPUT);

    Serial.println("Giving time for slave board to reset.");
    delay(6 * 1000);
  }
  else
  {
    Serial.println(F("Slave board"));
  }

  Serial.print("Waiting for first sync...");
  waitForSync();
  Serial.println(" Boards synced.");

  Serial.println("Initing external serial line");
  initSerial(Serial1);
  Serial.println("External serial line clear");


  isPassing = true;
  test();

  if (isPassing) {
    Serial.println("--------------------------------------------------");
    Serial.println("                  SUCCESS!!!                      ");
    Serial.println("--------------------------------------------------");    
    blinkSuccess();
  } else {
    Serial.println("--------------------------------------------------");
    Serial.println("                    Failure                       ");
    Serial.println("--------------------------------------------------");
    blinkFailure();
  }
}

void loop()
{
  // put your main code here, to run repeatedly:
}

bool compareBuffer(char *buffer, size_t s)
{
  Serial.print("compareBuffer size: ");
  Serial.println(s);

  for (size_t i = 0; i < s; i++)
  {
    int actual = buffer[i];
    int expected = getBufferByte(i);

    if (actual != expected) {
      Serial.print("idx ");
      Serial.print(i);
      Serial.print("\tActual: ");
      Serial.print(actual);
      Serial.print("\tExpected: ");
      Serial.print(expected);
      Serial.println("");

      return false;
    }
  }
  Serial.println("Compare buffer passed.");
  return true;
}

void test()
{
  if(!isPassing) {
    return;
  }
  
  waitForSync();

  if (isBoardMaster)
  {
    Serial.println("Sending...");
    fillBuffer(buffer, BUFFER_SIZE);
    int ret = xmodemTransmit(buffer, BUFFER_SIZE);
    Serial.print("Transmit result: ");
    Serial.println(ret);
    if (ret < 0) {
      isPassing = false;
    }
  }

  if (!isBoardMaster)
  {
    Serial.println("Receiving...");
    int ret = xmodemReceive(buffer, BUFFER_SIZE);
    Serial.print("Receive result: ");
    Serial.println(ret);
    if (ret < 0) {
      isPassing = false;
    }

    if (!compareBuffer(buffer, BUFFER_SIZE)) {
      Serial.println("Buffer compare failed");
      isPassing = false;
    }
  }
}
