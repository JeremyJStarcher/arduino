#include <Arduino.h>
#include <stdint.h>
#include "ioline.h"
#include "xmodem-crc.h"

#define MAXRETRANS 16

//* Control bytes passed back and forth
#define SOH_128 0x01  // 128 byte packet.
#define SOH_1024 0x02 // 1024 byte packet(unused)
#define CODE_EOT 0x04 // End of transmission
#define CODE_ACK 0x06 // ACK - Good
#define CODE_NAK 0x15 // Resend
#define CODE_CAN 0x18 // Cancel transmission

#define CHECKSOME_CRC 'C' // Use CRC checksomes
#define CHECKSOME_OLD CODE_NAK // use old checksomes.

// When XMODEM was first put together, CTRL-Z was the 'end of file'
// marker.  Packets should be padded with this character
#define CODE_PADDING 0x1A


#define INIT_FRAME_RETRY 1
#define INIT_FRAME_NEW 2

bool XmodemCrc::isDone() {
  return this->status != 0;
}

void XmodemCrc::nextTransmit(char *buf, size_t bytes) {
  if (this->status != 0) {
    return;
  }

  switch (this->state)
  {
    case XMODEM_STATE_T_INIT_TRANSMISSION:
      this->t_init_transmission();
      break;
    case XMODEM_STATE_T_PACKET:
      this->t_frame(buf, bytes);
      break;
    case XMODEM_STATE_T_EOT:
      this->t_eot();
      break;
  }
}

void XmodemCrc::nextRecieve(char *buf, size_t bytes) {
  if (this->status != 0) {
    return;
  }
  switch (this->state)
  {
    case XMODEM_STATE_R_SYNC:
      this->r_sync();
      break;
    case XMODEM_STATE_R_PACKET:
      this->r_frame(buf, bytes);
      break;
  }
}

void XmodemCrc::receive(IoLine *_serial, size_t packetSize) {
  //  this->buf = dest;
  this->bufSize = packetSize;
  this->packetNumber = 0;
  this->state = XMODEM_STATE_R_SYNC;
  this->status = XMODEM_STATUS_RUNNING;
  this->serial = _serial;
  this->tryChar = CHECKSOME_CRC;
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
    if (this->tryChar == CHECKSOME_CRC) {
      this->tryChar = CHECKSOME_OLD;
    }
  }

  signed int c;

  if (this->tryChar) _outbyte(this->tryChar);

  if ((c = _inbyte((XMODEM_TIMEOUT) << 1)) >= 0) {
    switch (c) {
      case SOH_128:
        this->state = XMODEM_STATE_R_PACKET;
        break;
#if 0
      case SOH_1024:
        this->state = XMODEM_STATE_R_PACKET;
        goto start_recv;
#endif
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

void XmodemCrc::r_frame(char *buf, size_t bytes) {
  this->triesLeft--;
  if (!this->triesLeft) {
    _outbyte(CODE_CAN);
    _outbyte(CODE_CAN);
    _outbyte(CODE_CAN);
    flushinput();
    this->status = -4; /* xmit error */
    return;
  }

  if (this->tryChar == CHECKSOME_CRC) this->useCrc = true;

  this->tryChar = 0;

  bool isRejected = false;

  int packetNumber = _inbyte(XMODEM_TIMEOUT);
  int complimentPacketNumber = _inbyte(XMODEM_TIMEOUT);

  if (packetNumber == -1) isRejected = true;
  if (complimentPacketNumber == -1) isRejected = true;

  for (int i = 0; i < bufSize; i++)
  {
    int ch = _inbyte(XMODEM_TIMEOUT);
    if (ch == -1) {
      isRejected = true;
      break;
    }

    if (i < bytes) {
      buf[i] = ch;
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
    this->hasData = false;
    this->init_frame(INIT_FRAME_RETRY);
  } else {
    this->hasData = true;
    _outbyte(CODE_ACK);

    this->state = XMODEM_STATE_R_SYNC;
    this->init_frame(INIT_FRAME_NEW);
  }
}

///////////////////////////////////////////////////////////
void XmodemCrc::transmit(IoLine *_serial, size_t packetSize) {
  //  this->buf = src;
  this->bufSize = packetSize;
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
      case CHECKSOME_CRC:
        this->useCrc = true;
        this->state = XMODEM_STATE_T_PACKET;
        break;
      case CHECKSOME_OLD:
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
  this->hasData = false;
  if (initMode == INIT_FRAME_NEW) {
    this->packetNumber++;
  }
}

void XmodemCrc::t_frame(char *buf, size_t bytes) {
  this->triesLeft--;
  if (!this->triesLeft) {
    _outbyte(CODE_CAN);
    _outbyte(CODE_CAN);
    _outbyte(CODE_CAN);
    flushinput();
    this->status = -4; /* xmit error */
    return;
  }

  _outbyte(SOH_128);
  _outbyte(this->packetNumber);
  _outbyte(~this->packetNumber);

  for (int i = 0; i < this->bufSize; i++)
  {
    //JJS TRY ME
    unsigned char ch = (i <= bytes) ? buf[i] : CODE_PADDING;
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
        if (bytes < this->bufSize) {
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
