#include <Arduino.h>
#include <stdint.h>
#include "ioline.h"
#include "xmodem-crc.h"

#define DLY_1S 1000
#define MAXRETRANS 3

#define CODE_SOH 0x01
#define CODE_STX 0x02
#define CODE_EOT 0x04
#define CODE_ACK 0x06
#define CODE_NAK 0x15
#define CODE_CAN 0x18
#define CODE_CTRLZ 0x1A

void XmodemCrc::next() {
  switch (this->state)
  {
    case XMODEM_STATE_T_SYNC:
      this->t_sync();
      break;
    case XMODEM_STATE_T_FRAME:
      this->t_frame();
      break;
  }
}

void XmodemCrc::transmit(
  IoLine *_serial,
  unsigned char
  *src, int srcSize
) {
  Serial.println("transmit");
  this->buf = src;
  this->bufSize = srcSize;
  this->pos = 0;
  this->retries = 16;
  this->packetNumber = 1;
  this->state = XMODEM_STATE_T_SYNC;
  this->status = XMODEM_STATUS_RUNNING;
  this->serial = _serial;
  this->len = 0;
}

void XmodemCrc::t_sync() {
  Serial.println("t_sync()");
  this->retries--;
  if (this->retries == 0) {
    _outbyte(CODE_CAN);
    _outbyte(CODE_CAN);
    _outbyte(CODE_CAN);
    flushinput();
    this->status = -2;
  }

  signed int c;
  if ((c = _inbyte((DLY_1S) << 1)) >= 0) {
    switch (c) {
      case 'C':
        Serial.println("USING CRC MODE");
        this->useCrc = true;
        this->state = XMODEM_STATE_T_FRAME;
        break;
      case CODE_NAK:
        Serial.println("USING CHECKSOME MODE");
        this->useCrc = false;
        this->state = XMODEM_STATE_T_FRAME;
        break;
      case CODE_CAN:
        if ((c = _inbyte(DLY_1S)) == CODE_CAN) {
          _outbyte(CODE_ACK);
          flushinput();
          this->status = -1; /* canceled by remote */
        }
        break;
      default:
        break;
    }
  }
}

void XmodemCrc::calcRunningChecksum(unsigned char ch) {
  this->crc = this->crc16_ccitt(crc, ch);
  this->ccks += ch;
  _outbyte(ch);
}

void XmodemCrc::t_frame() {

  // I've optimized for memory usage at the cost of run-time speed.
  // There are some messy things here that I'd normally never do, but
  // hey, this is embedded work/
  // No transmit buffer, so we'll calculate *and* transmit in the same breath

  if (this->pos > this->bufSize) {
    signed int c;

    for (int retry = 0; retry < 10; ++retry) {
      _outbyte(CODE_EOT);
      if ((c = _inbyte((DLY_1S) << 1)) == CODE_ACK) break;
    }
    flushinput();
    this->status = (c == CODE_ACK) ? len : -5;
    return;
  }

  for (int retry = 0; retry < MAXRETRANS; ++retry) {
    Serial.print("t_frame() #");
    Serial.print(this->packetNumber);
    Serial.print(" try ");
    Serial.println(retry);

    this->bytesInPacket = 0;
    this->txSize = 128;
    this->crc = 0; // "Modern" CRC checksum
    this->ccks = 0; // Original XModem checksum

    _outbyte(CODE_SOH);
    _outbyte(this->packetNumber);
    _outbyte(~this->packetNumber);

    Serial.print("CRC2 ");
    for (int i = 0; i < this->txSize; i++)
    {
      size_t p = this->pos + i;

      unsigned char ch = (p <= this->bufSize) ? this->buf[p] : CODE_CTRLZ;
      Serial.print((int) ch);
      Serial.print(" ");
      calcRunningChecksum(ch);
    }
    Serial.println("");
    Serial.print("CRC: ");
    Serial.println(this->crc);

    // WORKING VALUE
    // CRC CHARS: 65 66 67 69 68 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 48 49 50 51 52 53 54 55 48 65 66 67 69 68 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 48 49 50 51 52 53 54 55 48 65 66 67 69 68 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 48 49 50 51 52 53 54 55 48 65 66 67 69 68 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87
    //         C2 65 66 67 69 68 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 48 49 50 51 52 53 54 55 48 65 66 67 69 68 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 48 49 50 51 52 53 54 55 48 65 66 67 69 68 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 48 49 50 51 52 53 54 55 48 65 66 67 69 68 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87
    // 16703


    if (this->useCrc) {
      _outbyte((this->crc >> 8) & 0xFF);
      _outbyte(this->crc & 0xFF);
    } else {
      _outbyte(this->ccks);
    }

    signed int c;
    if ((c = _inbyte(DLY_1S)) >= 0 ) {
      switch (c) {
        case CODE_ACK:
          Serial.println("CODE_ACK");
          ++this->packetNumber;
          this->pos += this->txSize;
          // We stay in this state goto start_trans;
          return;
          break;
        case CODE_CAN:
          Serial.println("CODE_CAN");
          if ((c = _inbyte(DLY_1S)) == CODE_CAN) {
            _outbyte(CODE_ACK);
            flushinput();
            this->status = -1; /* canceled by remote */
          }
          return;
          break;
        case CODE_NAK:
          Serial.println("CODE_NAK");
          break;
        default:
          Serial.print("default, C = 0x");
          Serial.print(c, HEX);
          Serial.print(" ");
          Serial.println(c, DEC);
          break;
      }
    }
  }
  _outbyte(CODE_CAN);
  _outbyte(CODE_CAN);
  _outbyte(CODE_CAN);
  flushinput();
  Serial.println("XMIT ERROR");
  this->status = -4; /* xmit error */
}


// Raw IO routines

int XmodemCrc::_inbyte(int t) {
  return this->serial->readbyte(t);
}

void XmodemCrc::_outbyte(int b) {
  this->serial->writebyte(b);
  this->bytesInPacket++;
}

void XmodemCrc::flushinput(void)
{
  while (this->_inbyte(((DLY_1S) * 3) >> 1) >= 0)
    ;
}


unsigned short XmodemCrc::crc16_ccitt(unsigned short crc, unsigned char ch)
{
  int i;
  crc ^= ch << 8;
  for ( i = 0; i < 8; ++i ) {
    if ( crc & 0x8000 )
      crc = (crc << 1) ^ 0x1021;
    else
      crc = crc << 1;
  }

  return crc;
}
