#include "util.h"
//#define XMODEM_CRC_FAST 1
#define XMODEM_CRC_SLOW 1
#include "xmodem.h"

#include "./test/tests.cpp";
extern bool isPassing;

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

  long long start = millis();
  testAll(isBoardMaster);
  long long end = millis();
  long dur = end - start;

  Serial.print("Total time: ");
  Serial.print(dur);
  Serial.print("ms ");
  Serial.print(dur / 1000 / 60);
  Serial.println(" mins");

  if (isPassing)
  {
    Serial.println("--------------------------------------------------");
    Serial.println("                  SUCCESS!!!                      ");
    Serial.println("--------------------------------------------------");
    blinkSuccess();
  }
  else
  {
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

int serial_read(long int ms)
{

  const long long t = millis() + ms;
  int ch;
  while (1)
  {
    ch = Serial1.read();
    if (ch >= 0)
    {
      return ch;
    }

    if (millis() > t)
    {
      return -1;
    }
  }
}

void serial_write(int ch)
{
  Serial1.write(ch);
}
