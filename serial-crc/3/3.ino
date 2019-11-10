#define BUFFER_SIZE 2000
static unsigned char buffer[BUFFER_SIZE];

#include "util.h"

// #define XMODEM_LOG_SERIAL0
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

bool compareBuffer(unsigned char *buffer, size_t s)
{
  for (size_t i = 0; i < s; i++)
  {
    int actual = buffer[i];
    int expected = getBufferByte(i);

    if (actual != expected)
    {
      Serial.print("COMPARE FAILURE AT POSITION ");
      Serial.print(i);
      Serial.print(" Expected ");
      Serial.print(expected);
      Serial.print(" actual ");
      Serial.println(actual);

      return false;
    }
  }
  return true;
}

long kk = 0;
int serial_read(long int ms)
{

  const long long t = millis() + ms;
  int ch;
  while (1)
  {
    ch = Serial1.read();
    if (ch >= 0)
    {
      //Serial.print("(read ");
      //Serial.print(ch);
      //Serial.println(") ");
      kk += 1;
      if (kk == 90) continue;
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
  //Serial.print("(write ");
  //Serial.print(ch);
  //Serial.println(") ");
  Serial1.write(ch);
  delay(10);
}

void update_packet(XModemPacketStatus status)
{
	Serial.print(status.packetNumber);
	Serial.print(F("\t"));

	switch (status.action)
	{
	case XMODEM_PACKET_ACTION::Receiving:
		Serial.print(F("Receiving"));
		break;
	case XMODEM_PACKET_ACTION::Timeout:
		Serial.print(F("Timeout"));
		break;
	case XMODEM_PACKET_ACTION::PacketNumberCorrupt:
		Serial.print(F("PacketNumberCorrupt"));
		break;
	case XMODEM_PACKET_ACTION::PacketNumberOutOfSequence:
		Serial.print(F("PacketNumberOutOfSequence"));
		break;
	case XMODEM_PACKET_ACTION::CrcMismatch:
		Serial.print(F("CrcMismatch"));
		break;
	case XMODEM_PACKET_ACTION::ChecksomeMismatch:
		Serial.print(F("ChecksomeMismatch"));
		break;
	case XMODEM_PACKET_ACTION::Accepted:
		Serial.print(F("Accepted"));
		break;
	case XMODEM_PACKET_ACTION::ValidDuplicate:
		Serial.print(F("ValidDuplicate"));
		break;
	case XMODEM_PACKET_ACTION::ReceiverACK:
		Serial.print(F("ReceiverACK"));
		break;
	case XMODEM_PACKET_ACTION::ReceiverNAK:
		Serial.print(F("ReceiverNAK"));
		break;
	case XMODEM_PACKET_ACTION::ReceiverGarbage:
		Serial.print(F("ReceiverGarbage"));
		break;
	case XMODEM_PACKET_ACTION::Sync:
		Serial.print(F("Sync"));
		break;
	case XMODEM_PACKET_ACTION::SyncError:
		Serial.print(F("SyncError"));
		break;
	case XMODEM_PACKET_ACTION::Transmitting:
		Serial.print(F("Transmitting"));
		break;
	case XMODEM_PACKET_ACTION::WaitingForReceiver:
		Serial.print(F("WaitingForReceiver"));
		break;
	default:
		Serial.print(F("Unknown Action"));
		break;
	}
	Serial.println(F(""));
}


void test()
{
  if (!isPassing)
  {
    return;
  }

  waitForSync();

  Xmodem xmodem(serial_read, serial_write);

  if (isBoardMaster)
  {
    Serial.println("Sending...");
    fillBuffer(buffer, BUFFER_SIZE);
    XMODEM_TRANSFER_STATUS ret = xmodem.transmitFullBuffer(buffer, BUFFER_SIZE, update_packet);
    Serial.print("Transmit result: ");
    //Serial.println(ret);
    if (ret != XMODEM_TRANSFER_STATUS::SUCCESS)
    {
      isPassing = false;
    }
  }

  if (!isBoardMaster)
  {
    Serial.println("Receiving...");
    XMODEM_TRANSFER_STATUS ret = xmodem.receiveFullBuffer(buffer, BUFFER_SIZE, update_packet);
    Serial.print("Receive result: ");
    //Serial.println(ret);
    if (ret != XMODEM_TRANSFER_STATUS::SUCCESS)
    {
      isPassing = false;
    }

    if (!compareBuffer(buffer, BUFFER_SIZE))
    {
      Serial.println("Buffer compare failed");
      isPassing = false;
    }
  }
}
