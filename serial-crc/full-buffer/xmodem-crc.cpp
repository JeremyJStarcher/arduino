
#include <Arduino.h>
#include <stdint.h>
#include "ioline.h"
#include "xmodem-crc.h"

#define MAXRETRANS 16

#define CODE_SOH 0x01
#define CODE_STX 0x02
#define CODE_EOT 0x04
#define CODE_ACK 0x06
#define CODE_NAK 0x15
#define CODE_CAN 0x18
#define CODE_CTRLZ 0x1A

#define INIT_FRAME_RETRY 1
#define INIT_FRAME_NEW 2

bool XmodemCrc::isDone() {
  return this->status != 0;
}

void XmodemCrc::next() {
  if (this->status != 0) {
    return;
  }
  switch (this->state)
  {
    case XMODEM_STATE_T_INIT_TRANSMISSION:
      this->t_init_transmission();
      break;
    case XMODEM_STATE_T_PACKET:
      this->t_frame();
      break;
    case XMODEM_STATE_T_EOT:
      this->t_eot();
      break;
    case XMODEM_STATE_R_SYNC:
      this->r_sync();
      break;
    case XMODEM_STATE_R_PACKET:
      this->r_frame();
      break;

  }
}

void XmodemCrc::receive(IoLine *_serial, unsigned char *dest, int destSize) {
  this->buf = dest;
  this->bufSize = destSize;
  this->pos = 0;
  this->packetNumber = 0;
  this->state = XMODEM_STATE_R_SYNC;
  this->status = XMODEM_STATUS_RUNNING;
  this->serial = _serial;
  this->tryChar = 'C';
  this->state = XMODEM_STATE_R_SYNC;
  this->init_frame(INIT_FRAME_NEW);
}

void XmodemCrc::r_sync() {
  this->triesLeft--;
  if (this->triesLeft == 0) {
    _outbyte(CODE_CAN);
    _outbyte(CODE_CAN);
    _outbyte(CODE_CAN);
    flushinput();
    this->status = -2;
    return;
  }

  if (this->triesLeft < MAXRETRANS / 2) {
    if (this->tryChar == 'C') {
      this->tryChar = CODE_NAK;
    }
  }

  signed int c;

  if (this->tryChar) _outbyte(this->tryChar);

  if ((c = _inbyte((XMODEM_TIMEOUT) << 1)) >= 0) {
    switch (c) {
      case CODE_SOH:
        this->state = XMODEM_STATE_R_PACKET;
        break;
      // goto start_recv;
      //    case XMODEM_STX:
      //      bufsz = 1024;
      //      goto start_recv;
      case CODE_EOT:
        flushinput();
        _outbyte(CODE_ACK);
        this->status = 1;
        break;
      case CODE_CAN:
        if ((c = _inbyte(XMODEM_TIMEOUT)) == CODE_CAN) {
          flushinput();
          _outbyte(CODE_ACK);
          this->status = -1;/* canceled by remote */
        }
        break;
      default:
        break;
    }
  }
}

void XmodemCrc::r_frame() {
  this->triesLeft--;
  if (!this->triesLeft) {
    _outbyte(CODE_CAN);
    _outbyte(CODE_CAN);
    _outbyte(CODE_CAN);
    flushinput();
    this->status = -4; /* xmit error */
    return;
  }

  if (this->tryChar == 'C') this->useCrc = true;

  this->tryChar = 0;

  bool isRejected = false;

  int packetNumber = _inbyte(XMODEM_TIMEOUT);
  int complimentPacketNumber = _inbyte(XMODEM_TIMEOUT);

  if (packetNumber == -1) isRejected = true;
  if (complimentPacketNumber == -1) isRejected = true;

  for (int i = 0; i < this->txSize; i++)
  {
    size_t p = this->pos + i;
    int ch = _inbyte(XMODEM_TIMEOUT);

    if (ch == -1) {
      isRejected = true;
      break;
    }

    if (p < this->bufSize) {
      this->buf[p] = ch;
    }
    calcRunningChecksum(ch);
  }

  if (this->useCrc) {
    int crcHigh =  _inbyte(XMODEM_TIMEOUT);
    int crcLow =  _inbyte(XMODEM_TIMEOUT);

    int crc = (crcHigh * 256) + crcLow;

    if (crc != this->crc) isRejected = true;
  } else {
    int ccks = _inbyte(XMODEM_TIMEOUT);
    if (ccks != this->ccks) isRejected = true;
  }

  if (isRejected) {
    this->packetNumber--; // init increments it.
    this->init_frame(INIT_FRAME_RETRY);
  } else {
    _outbyte(CODE_ACK);
    this->pos += this->txSize;
  }
  this->state = XMODEM_STATE_R_SYNC;
  this->init_frame(INIT_FRAME_NEW);

}

///////////////////////////////////////////////////////////
void XmodemCrc::transmit(IoLine *_serial, unsigned char *src, int srcSize) {
  this->buf = src;
  this->bufSize = srcSize;
  this->pos = 0;
  this->packetNumber = 0;
  this->state = XMODEM_STATE_T_INIT_TRANSMISSION;
  this->status = XMODEM_STATUS_RUNNING;
  this->serial = _serial;
  this->init_frame(INIT_FRAME_NEW);
}

void XmodemCrc::t_init_transmission() {
  this->triesLeft--;
  if (this->triesLeft == 0) {
    _outbyte(CODE_CAN);
    _outbyte(CODE_CAN);
    _outbyte(CODE_CAN);
    flushinput();
    this->status = -2;
    return;
  }

  signed int c;
  if ((c = _inbyte((XMODEM_TIMEOUT) << 1)) >= 0) {
    switch (c) {
      case 'C':
        this->useCrc = true;
        this->state = XMODEM_STATE_T_PACKET;
        break;
      case CODE_NAK:
        this->useCrc = false;
        this->state = XMODEM_STATE_T_PACKET;
        break;
      case CODE_CAN:
        if ((c = _inbyte(XMODEM_TIMEOUT)) == CODE_CAN) {
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
}

void XmodemCrc::init_frame(char initMode) {
  this->crc = 0;
  this->ccks = 0;
  this->triesLeft = MAXRETRANS;
  this->txSize = XMODEM_BLOCKSIZE;
  if (initMode == INIT_FRAME_NEW) {
    this->packetNumber++;
  }
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

  _outbyte(CODE_SOH); // Hardcoded as 128BYTE packet.
  _outbyte(this->packetNumber);
  _outbyte(~this->packetNumber);

  for (int i = 0; i < this->txSize; i++)
  {
    size_t p = this->pos + i;

    unsigned char ch = (p <= this->bufSize) ? this->buf[p] : CODE_CTRLZ;
    calcRunningChecksum(ch);
    _outbyte(ch);
  }

  if (this->useCrc) {
    _outbyte((this->crc >> 8) & 0xFF);
    _outbyte(this->crc & 0xFF);
  } else {
    _outbyte(this->ccks);
  }

  signed int c;
  if ((c = _inbyte(XMODEM_TIMEOUT)) >= 0 ) {
    switch (c) {
      case CODE_ACK:
        this->init_frame(INIT_FRAME_NEW);
        this->pos += this->txSize;

        if (this->pos >= this->bufSize) {
          this->state = XMODEM_STATE_T_EOT;
        }
        return;
      case CODE_CAN:
        if ((c = _inbyte(XMODEM_TIMEOUT)) == CODE_CAN) {
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

void XmodemCrc::t_eot() {
  signed int c;

  for (int retry = 0; retry < MAXRETRANS; ++retry) {
    _outbyte(CODE_EOT);
    if ((c = _inbyte((XMODEM_TIMEOUT) << 1)) == CODE_ACK) break;
  }
  flushinput();
  this->status = (c == CODE_ACK) ? 1 : -5;
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
  while (this->_inbyte(((XMODEM_TIMEOUT) * 3) >> 1) >= 0)
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

unsigned char XmodemCrc::getPacketNumber() {
  return this->packetNumber;
}

unsigned char XmodemCrc::getState() {
  return this->state;
}

signed char XmodemCrc::getStatus() {
  return this->status;
}
