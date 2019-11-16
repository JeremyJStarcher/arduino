/* This is the MEGA to MEGA example / test.
 *  
 *  This requires two MEGAS. One configured as a master and configured as a slave.
 *  
 *  This script can be deployed from the Arduino IDE, but it is easier to use the
 *  arduino-cli to deploy since the script file can deploy to both Arduinos and
 *  and automatically pull up a terminal.
 *  
 *  See the library's `extras` directory for details.
 *  
 *  Wiring between two MEGAs
 *  M = Master Board
 *  S = Slave Board
 *  
 *     Purpose        From              To
 * _______________________________________________
 * | DataLink      | [RX1] M18    | [TX1] S19    |
 * | DataLink      | [TX1] M19    | [RX1] S18    |
 * | Slave RST     |       M8     |       S-RST  | 
 * | Ground        |       M-GND  |       S-GND  |
 * | Config Master |       M-A2   |       M-+5   |
 * | Config Slave  |       S-A2   |       S-GND  |
 * | Handshaking   |       M-7    |       S-6    |
 * | Handshaking   |       S-6    |       M-7    |
 * +---------------+--------------+--------------+
*/


#include "util.h"
#include <xmodem.h>


extern testAll(bool isBoardMaster);

extern bool isPassing;

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
    Serial.println(F("Giving slave time to be programmed"));
    delay(10 * 1000);

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

  Serial.print((dur / 1000) / 60);
  Serial.print(" mins ");
  Serial.print((dur / 1000) % 60);
  Serial.println(" secs");

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
