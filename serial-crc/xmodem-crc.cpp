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

bool XmodemCrc::isDone() {
  return this->status != 0;
}

void XmodemCrc::next() {
  if (this->status != 0) {
    return;
  }
  switch (this->state)
  {
    case XMODEM_STATE_T_SYNC:
      this->t_sync();
      break;
    case XMODEM_STATE_T_FRAME:
      this->t_frame();
      break;
    case XMODEM_STATE_T_EOT:
      this->t_eot();
      break;
  }
}

void XmodemCrc::transmit(
  IoLine *_serial,
  unsigned char
  *src, int srcSize
) {
  this->buf = src;
  this->bufSize = srcSize;
  this->pos = 0;
  this->packetNumber = 0;
  this->state = XMODEM_STATE_T_SYNC;
  this->status = XMODEM_STATUS_RUNNING;
  this->serial = _serial;
  this->t_init_frame();
}

void XmodemCrc::t_sync() {
  this->triesLeft--;
  if (this->triesLeft == 0) {
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
        this->useCrc = true;
        this->state = XMODEM_STATE_T_FRAME;
        break;
      case CODE_NAK:
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

void XmodemCrc::t_init_frame() {
  this->triesLeft = MAXRETRANS;
  this->packetNumber++;
}

void XmodemCrc::t_eot() {
  signed int c;

  for (int retry = 0; retry < MAXRETRANS; ++retry) {
    _outbyte(CODE_EOT);
    if ((c = _inbyte((DLY_1S) << 1)) == CODE_ACK) break;
  }
  flushinput();
  this->status = (c == CODE_ACK) ? 1 : -5;
}

void XmodemCrc::t_frame() {

  // I've optimized for memory usage at the cost of run-time speed.
  // There are some messy things here that I'd normally never do, but
  // hey, this is embedded work/
  // No transmit buffer, so we'll calculate *and* transmit in the same breath

  this->triesLeft--;
  if (!this->triesLeft) {
    _outbyte(CODE_CAN);
    _outbyte(CODE_CAN);
    _outbyte(CODE_CAN);
    flushinput();
    this->status = -4; /* xmit error */
    return;
  }

  this->bytesInPacket = 0;
  this->txSize = 128;
  this->crc = 0; // "Modern" CRC checksum
  this->ccks = 0; // Original XModem checksum

  _outbyte(CODE_SOH);
  _outbyte(this->packetNumber);
  _outbyte(~this->packetNumber);

  for (int i = 0; i < this->txSize; i++)
  {
    size_t p = this->pos + i;

    unsigned char ch = (p <= this->bufSize) ? this->buf[p] : CODE_CTRLZ;
    calcRunningChecksum(ch);
  }

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
        this->t_init_frame();
        this->pos += this->txSize;

        if (this->pos >= this->bufSize) {
          this->state = XMODEM_STATE_T_EOT;
        }
        return;
      case CODE_CAN:
        if ((c = _inbyte(DLY_1S)) == CODE_CAN) {
          _outbyte(CODE_ACK);
          flushinput();
          this->status = -1; /* canceled by remote */
        }
        return;
      case CODE_NAK:
        return;
      default:
        break;
    }
  }
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
