#include <Arduino.h>
#include <stdint.h>
#include "ioline.h"
#include "xmodem-crc.h"

#if 1
#define LOGNL(x) Serial.println(x)
#define LOG(x) Serial.print("");Serial.print(x);Serial.print("")
#else
#define LOGNL(x)
#define LOG(x)
#endif


#define MAXRETRANS 6
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

int XMODEM_SEND_EXTRA_CHARS;
int XMODEM_SEND_EAT_CHARS;
int XMODEM_BREAK_CRC;

bool XmodemCrc::isDone() {
  return this->status != 0;
}

void XmodemCrc::nextTransmit(char *buf, size_t bytes, bool isEot) {
  switch (this->state)
  {
    case XMODEM_STATE_T_INIT_TRANSMISSION:
      this->t_init_transmission();
      break;
    case XMODEM_STATE_T_PACKET:
      this->t_frame(buf, bytes, isEot);
      break;
    case XMODEM_STATE_T_WAIT_REPLY:
      this->t_waitReply(bytes, isEot);
      break;
    case XMODEM_STATE_T_EOT:
      this->t_eot();
      break;
  }
}

void XmodemCrc::nextRecieve(char *buf, size_t bytes) {
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
    _outbyte(true, CODE_CAN);
    _outbyte(true, CODE_CAN);
    _outbyte(true, CODE_CAN);
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

  if (this->tryChar) _outbyte(true, this->tryChar);

  if ((c = _inbyte(true, (XMODEM_TIMEOUT) << 1)) >= 0) {
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
        _outbyte(true, CODE_ACK);
        this->status = 1;
        break;
      case CODE_CAN:
        if ((c = _inbyte(true, XMODEM_TIMEOUT)) == CODE_CAN) {
          flushinput();
          _outbyte(true, CODE_ACK);
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
    _outbyte(true, CODE_CAN);
    _outbyte(true, CODE_CAN);
    _outbyte(true, CODE_CAN);
    flushinput();
    this->status = -4; /* xmit error */
    return;
  }

  if (this->tryChar == CHECKSOME_CRC) this->useCrc = true;

  this->tryChar = 0;

  bool isRejected = false;

  int packetNumber = _inbyte(true, XMODEM_TIMEOUT);
  int complimentPacketNumber = _inbyte(true, XMODEM_TIMEOUT);

  if (packetNumber == -1) isRejected = true;
  if (complimentPacketNumber == -1) isRejected = true;

  if (packetNumber != this->packetNumber) {
    isRejected = true;
  }

  LOG("<inbuffer>");

  for (int i = 0; i < bufSize; i++)
  {
    int ch = _inbyte(false, XMODEM_TIMEOUT);
    if (i < bytes) {
      buf[i] = ch;
    }
    LOG(ch); LOG(" ");
    calcRunningChecksum(ch);
  }
  LOG("<outbuffer>");

  if (this->useCrc) {
    int crcHigh =  _inbyte(true, XMODEM_TIMEOUT);
    int crcLow =  _inbyte(true, XMODEM_TIMEOUT);

    int crc = (crcHigh * 256) + crcLow;

    if (crc != this->crc) {
      isRejected = true;
    }
  } else {
    int ccks = _inbyte(true, XMODEM_TIMEOUT);
    if (ccks != this->ccks) isRejected = true;
  }

  flushinput();

  if (isRejected) {
    _outbyte(true, CODE_NAK);
    this->hasData = false;
    this->state = XMODEM_STATE_R_SYNC;

    this->init_frame(INIT_FRAME_RETRY);
  } else {
    this->hasData = true;
    _outbyte(true, CODE_ACK);

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
    _outbyte(true, CODE_CAN);
    _outbyte(true, CODE_CAN);
    _outbyte(true, CODE_CAN);
    flushinput();
    this->status = -2;
    return;
  }

  signed int c;
  if ((c = _inbyte(true, (XMODEM_TIMEOUT) << 1)) >= 0) {
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
        if ((c = _inbyte(true, XMODEM_TIMEOUT)) == CODE_CAN) {
          _outbyte(true, CODE_ACK);
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

void XmodemCrc::t_frame(char *buf, size_t bytes, bool isEot) {
  this->triesLeft--;
  if (!this->triesLeft) {
    _outbyte(true, CODE_CAN);
    _outbyte(true, CODE_CAN);
    _outbyte(true, CODE_CAN);
    flushinput();
    this->status = -4; /* xmit error */
    return;
  }

  _outbyte(true, SOH_128);
  _outbyte(true, this->packetNumber);
  _outbyte(true, ~this->packetNumber);

  LOG("<outbuffer>");
  for (int i = 0; i < this->bufSize; i++)
  {
    unsigned char ch = (i <= bytes) ? buf[i] : CODE_PADDING;
    calcRunningChecksum(ch);
    if (XMODEM_SEND_EAT_CHARS > 0) {
      XMODEM_SEND_EAT_CHARS -= 1;
    } else {
      _outbyte(false, ch);
      LOG(ch);
      LOG(" ");
    }
  }
  LOGNL("<outbuffer>");

  while (XMODEM_SEND_EXTRA_CHARS > 0) {
    XMODEM_SEND_EXTRA_CHARS -= 1;
    LOG("<0>");
    _outbyte(true, 0);
  }

  if (this->useCrc) {
    _outbyte(true, (this->crc >> 8) & 0xFF);
    _outbyte(true, this->crc & 0xFF);
  } else {
    _outbyte(true, this->ccks);
  }
  this->state = XMODEM_STATE_T_WAIT_REPLY;
}


void XmodemCrc::t_waitReply(size_t bytes, bool isEot) {
  signed int c;

  if ((c = _inbyte(true, XMODEM_TIMEOUT)) > 0) {
    switch (c) {
        Serial.print("Waiting for reply");
        Serial.println(c);

      case CODE_ACK:
        Serial.print("ACK! ");
        Serial.print(bytes);
        Serial.print(" ");
        Serial.println(this->bufSize);

        this->state = XMODEM_STATE_T_PACKET;
        if (isEot) {
          this->state = XMODEM_STATE_T_EOT;
        }
        this->init_frame(INIT_FRAME_NEW);
        flushinput();
        return;
      case CODE_CAN:
        if ((c = _inbyte(true, XMODEM_TIMEOUT)) == CODE_CAN) {
          _outbyte(true, CODE_ACK);
          flushinput();
          this->status = -1; /* canceled by remote */
        }
        return;
      case CODE_NAK:
        this->state = XMODEM_STATE_T_PACKET;
        this->init_frame(INIT_FRAME_RETRY);
        flushinput();
        return;
      default:
        break;
    }
  }
}

void XmodemCrc::t_eot() {
  signed int c;

  for (int retry = 0; retry < MAXRETRANS; ++retry) {
    _outbyte(true, CODE_EOT);
    if ((c = _inbyte(true, (XMODEM_TIMEOUT) << 1)) == CODE_ACK) break;
  }
  flushinput();
  Serial.print("Final character was ");
  Serial.println(c);
  this->status = (c == CODE_ACK) ? 1 : -5;
}

// Raw IO routines

int XmodemCrc::_inbyte(bool doLog, int t) {
  int ch = this->serial->readbyte(t);
  if (doLog) {
    LOG("<in>\t"); LOGNL(ch);
  }
  return ch;
}

void XmodemCrc::_outbyte(bool doLog, int b) {
  if (doLog) {
    LOG("<out>\t"); LOGNL(b);
  }
  this->serial->writebyte(b);
}

void XmodemCrc::flushinput(void)
{
  LOG("<flushing buffer>");
  int count = 0;
  int c;
  while ((c = this->_inbyte(false, ((XMODEM_TIMEOUT) * 3) >> 1)) >= 0) {
    count++;
    LOG(c); LOG(" ");
  }
  LOG("Flushed: "); LOGNL(count);
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
