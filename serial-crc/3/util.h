#include <HardwareSerial.h>
#define READ_TIMEOUT 1000

#define INIT_SOH 0xA0
#define INIT_ACK 0xFE

extern void serial_write(int ch);
extern int serial_read(long int ms);

const int SERIAL_BAUD = 28800;
const int USB_BAUD = 9600;

bool isBoardMaster = false;
const int slaveResetPin = 8;
const int masterSelectPin = A2;
const int clearToSendPin = 7;
const int requestToSendPin = 6;

void waitForSync()
{
  digitalWrite(requestToSendPin, HIGH);
  while (digitalRead(clearToSendPin) == LOW)
  {
    ;
  }

  digitalWrite(requestToSendPin, LOW);
}

void blinkFailure()
{
  pinMode(LED_BUILTIN, OUTPUT);

  const int blinkRate = 100;
  while (true)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(blinkRate);
    digitalWrite(LED_BUILTIN, LOW);
    delay(blinkRate);
  }
}

void blinkSuccess()
{
  pinMode(LED_BUILTIN, OUTPUT);

  const int blinkRate = 100;
  while (true)
  {
    waitForSync();
    digitalWrite(LED_BUILTIN, HIGH);
    delay(blinkRate * 4);
    digitalWrite(LED_BUILTIN, LOW);
    delay(blinkRate);
  }
}

void initSerialInner(HardwareSerial remote, int snd, int rcv)
{
  if (snd)
  {
    unsigned int c;

    while ((c = serial_read(READ_TIMEOUT)) != INIT_ACK)
    {
      serial_write(INIT_SOH);
    }
  }

  if (rcv)
  {
    signed int c;

    while ((c = serial_read(READ_TIMEOUT)) != INIT_SOH)
    {
      delay(100);
    }
    serial_write(INIT_ACK);
  }

  Serial.print("Flushing serial port...");
  // Make sure the other side sends everything.
  for (int i = 0; i < 10; i++) {
    delay(100);
    while (serial_read(READ_TIMEOUT) != -1)
    {
      // Do onthing.
    }
  }
  
  Serial.println(serial_read(READ_TIMEOUT));
}

void initSerial(HardwareSerial remote)
{
  /*
     Arduino MEGA work around:

     The MEGA has this weird issue...

     Sometimes the serial ports will show traffic of all NILs (0x00) until there
     has been real traffic on the port.

     Some have recommended a 4.7K pull up resister on the RX line as a work around
     and I've heard there has been a lot of success with that.

     For my purposes, I'll just send traffic until the serial ports settle down.
  */

  if (isBoardMaster)
  {
    initSerialInner(remote, 1, 0);
  }
  else
  {
    initSerialInner(remote, 0, 1);
  }
}
