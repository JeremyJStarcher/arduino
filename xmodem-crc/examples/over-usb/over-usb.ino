#include <EEPROM.h>
#include <xmodem.h>

void setup()
{
  Serial.begin(9600);
  Serial.println(F("\n\n\nUSB Connection established"));

  // Serial3.begin(9600);
  // Serial3.println(F("\n\n\nUSB Connection established"));

  showHelp();
}

void loop()
{
  if (Serial.available() > 0) {

    int incomingByte = Serial.read(); // read the incoming byte:

    switch (incomingByte) {
      case 10:
      case 13:
        break;
      case '?':
        showHelp();
        break;
      case 'F':
      case 'f':
        fillEEPROM();
        showHelp();
        break;
      case 'C':
      case 'c':
        Serial.print("EEPROM CRC: ");
        Serial.println(eeprom_crc(), HEX);
        showHelp();
        break;
      case 'U':
      case 'u':
        upload();
        showHelp();
        break;
      case 'D':
      case 'd':
        download();
        showHelp();
        break;
      default:
        Serial.print(" I received:");
        Serial.println(incomingByte);
        showHelp();
    }
  }
}

int serial_read(long int ms)
{

  const long long t = millis() + ms;
  int ch;
  while (1)
  {
    ch = Serial.read();
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
  Serial.write(ch);
}

void showHelp() {
  Serial.println(F("\n\n? - Help"));
  Serial.println(F("F - Fill EEPROM with random data"));
  Serial.println(F("C - Show EEPROM CRC"));
  Serial.println(F("D - Download (xmodem)"));
  Serial.println(F("U - Upload (xmodem)"));

}

void fillEEPROM() {
  Serial.print("Filling EEPROM with data.  EEPROM size: ");
  Serial.println(EEPROM.length());

  for (int index = 0; index < EEPROM.length(); index++) {
    int val = analogRead(0) / 4;
    EEPROM.update(index, val);
  }
}

unsigned long eeprom_crc(void) {
  const unsigned long crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };

  unsigned long crc = ~0L;

  for (int index = 0 ; index < EEPROM.length()  ; ++index) {
    crc = crc_table[(crc ^ EEPROM[index]) & 0x0f] ^ (crc >> 4);
    crc = crc_table[(crc ^ (EEPROM[index] >> 4)) & 0x0f] ^ (crc >> 4);
    crc = ~crc;
  }
  return crc;
}


int getCharFromEEPROM(xmodem_t offset, xmodem_t i)
{
  xmodem_t pos = offset + i;
  if (pos < EEPROM.length())
  {
    return EEPROM.read(pos);
  }
  else
  {
    return -1;
  }
}

void putCharInEEPROM(xmodem_t offset, xmodem_t i, unsigned char ch)
{
  xmodem_t pos = offset + i;
  if (pos < EEPROM.length())
  {
    Serial3.print(pos, HEX);
    Serial3.print(" ");
    Serial3.print(ch, HEX);
    Serial3.println("");
    
    EEPROM.update(pos, ch);
  }
}



void update_packet(XModemPacketStatus status)
{
  switch (status.action)
  {
    case XMODEM_PACKET_ACTION::Receiving:
      // Serial3.println("XMODEM_PACKET_ACTION::Receiving");
      break;
    case XMODEM_PACKET_ACTION::Timeout:
      // Serial3.println(" XMODEM_PACKET_ACTION::Timeout:");
      break;
    case XMODEM_PACKET_ACTION::PacketNumberCorrupt:
      // Serial3.println(" XMODEM_PACKET_ACTION::PacketNumberCorrupt:");
      break;
    case XMODEM_PACKET_ACTION::PacketNumberOutOfSequence:
      // Serial3.println("XMODEM_PACKET_ACTION::PacketNumberOutOfSequence:");
      break;
    case XMODEM_PACKET_ACTION::CrcMismatch:
      // Serial3.println("XMODEM_PACKET_ACTION::CrcMismatch:");
      break;
    case XMODEM_PACKET_ACTION::ChecksomeMismatch:
      // Serial3.println("XMODEM_PACKET_ACTION::ChecksomeMismatch:");
      break;
    case XMODEM_PACKET_ACTION::Accepted:
      // Serial3.println("XMODEM_PACKET_ACTION::Accepted:");
      break;
    case XMODEM_PACKET_ACTION::ValidDuplicate:
      // Serial3.println("XMODEM_PACKET_ACTION::ValidDuplicate:");
      break;
    case XMODEM_PACKET_ACTION::ReceiverACK:
      // Serial3.println("XMODEM_PACKET_ACTION::ReceiverACK:");
      break;
    case XMODEM_PACKET_ACTION::ReceiverNAK:
      // Serial3.println("XMODEM_PACKET_ACTION::ReceiverNAK:");
      break;
    case XMODEM_PACKET_ACTION::ReceiverGarbage:
      // Serial3.println("XMODEM_PACKET_ACTION::ReceiverGarbage:");
      break;
    case XMODEM_PACKET_ACTION::Sync:
      // Serial3.println("XMODEM_PACKET_ACTION::Sync:");
      break;
    case XMODEM_PACKET_ACTION::SyncError:
      // Serial3.println("XMODEM_PACKET_ACTION::SyncError:");
      break;
    case XMODEM_PACKET_ACTION::Transmitting:
      // Serial3.println("XMODEM_PACKET_ACTION::Transmitting:");
      break;
    case XMODEM_PACKET_ACTION::WaitingForReceiver:
      // Serial3.println("XMODEM_PACKET_ACTION::WaitingForReceiver:");
      break;
    default:
      // Serial3.println("default");
      break;
  }
}

void upload(void) {
  Xmodem xmodem(serial_read, serial_write);
  int k = (int) xmodem.receiveCharacterMode(putCharInEEPROM, update_packet);
}

void download(void) {
  Serial.println("Starting download.....");
  Xmodem xmodem(serial_read, serial_write);
  int k = (int) xmodem.transmitCharacterMode(getCharFromEEPROM, update_packet);
  Serial.print("download complete. status ");
  Serial.println(k);
}
