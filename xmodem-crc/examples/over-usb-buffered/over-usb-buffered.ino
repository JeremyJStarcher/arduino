/*
    This version works on a buffered basis.  An entire packet is read and then
    is processed when the buffer is read correctly.

    Advantage:
      When processing invalid data would cause a problem, such as saving
      to a stream, then makes sure the buffer is valid before saving it.

    Disadvantage:
      Requires the RAM space for a buffer.
*/


#include <EEPROM.h>
#include <xmodem.h>
const size_t BUFFER_SIZE = 128;
unsigned char buffer[BUFFER_SIZE];
size_t transmitIndex = 0;
bool transmitEOF = false;

void setup()
{
  Serial.begin(9600);
  Serial.println(F("\n\n\nUSB Connection established"));

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

int Serial_read(long int ms)
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

void Serial_write(int ch)
{
  Serial.write(ch);
}

void showHelp() {
  Serial.println(F("\n\n? - **BUFFERED! ** Help"));
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

int getCharFromBuffer(xmodem_t offset, xmodem_t i)
{
  size_t pos = offset + i;
  if (!transmitEOF)
  {
    return buffer[i];
  }
  else
  {
    return -1;
  }
}

void putCharInBuffer(xmodem_t offset, xmodem_t i, unsigned char ch)
{
  buffer[i] = ch;
}

void update_packet(XModemPacketStatus status)
{
  /* In the character by character mode, these values are only
      used to update the UI, if applicable.
  */
  switch (status.action)
  {
    case XMODEM_PACKET_ACTION::Receiving:
      break;
    case XMODEM_PACKET_ACTION::Timeout:
      break;
    case XMODEM_PACKET_ACTION::PacketNumberCorrupt:
      break;
    case XMODEM_PACKET_ACTION::PacketNumberOutOfSequence:
      break;
    case XMODEM_PACKET_ACTION::CrcMismatch:
      break;
    case XMODEM_PACKET_ACTION::ChecksomeMismatch:
      break;
    case XMODEM_PACKET_ACTION::Accepted:
      saveBuffer();
      break;
    case XMODEM_PACKET_ACTION::ValidDuplicate:
      break;
    case XMODEM_PACKET_ACTION::ReceiverACK:
      // The other end accepted the packet.
      loadBuffer();
      break;
    case XMODEM_PACKET_ACTION::ReceiverNAK:
      break;
    case XMODEM_PACKET_ACTION::ReceiverGarbage:
      break;
    case XMODEM_PACKET_ACTION::Sync:
      break;
    case XMODEM_PACKET_ACTION::SyncError:
      break;
    case XMODEM_PACKET_ACTION::Transmitting:
      break;
    case XMODEM_PACKET_ACTION::WaitingForReceiver:
      break;
    default:
      break;
  }
}

void upload(void) {
  Serial.println("Starting Upload");
  Serial.println("If you are uploading a replacement EEPROM file,");
  Serial.println("there may be a lot of errors and retries.  Updating EEPROM");
  Serial.println("values takes a /long/ time.");

  transmitEOF = false;
  Xmodem xmodem(Serial_read, Serial_write);
  int k = (int) xmodem.receiveCharacterMode(putCharInBuffer, update_packet);
}

void download(void) {
  Serial.println("Starting download.....");
  Xmodem xmodem(Serial_read, Serial_write);

  transmitEOF = false;
  loadBuffer();
  int k = (int) xmodem.transmitCharacterMode(getCharFromBuffer, update_packet);
  Serial.print("download complete. status ");
  Serial.println(k);
}

void loadBuffer() {
  for (int i = 0; i < BUFFER_SIZE; i++) {
    size_t pos = transmitIndex + i;
    if (pos < EEPROM.length()) {
      buffer[i] = EEPROM.read(pos);
    } else {
      transmitEOF = true;
    }
  }
  transmitIndex += BUFFER_SIZE;
}

void saveBuffer() {
  for (int i = 0; i < BUFFER_SIZE; i++) {
    size_t pos = transmitIndex + i;
    if (pos < EEPROM.length())
    {
      EEPROM.update(pos, buffer[i]);
    }
  }
  debug_blink();
  transmitIndex += BUFFER_SIZE;
}

void debug_blink() {
  for (int i = 0; i < 20; i++) {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(250);                        // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(250);                        // wait for a second
  }
}
