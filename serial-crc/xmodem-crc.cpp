#define BLOCKSIZE 16

#include <Arduino.h>
#include <stdint.h>
#include "ioline.h"
#include "xmodem-crc.h"

// TODO
// merge the init_frame code if it turns out those are the same.

#define DLY_1S 1000
#define MAXRETRANS 3

#define CODE_SOH 0xFF // 0x01
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
    case XMODEM_STATE_R_SYNC:
      this->r_sync();
      break;
    case XMODEM_STATE_R_FRAME:
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
  this->r_init_frame();
}

void XmodemCrc::r_init_frame() {
  this->crc = 0;
  this->ccks = 0;
  this->triesLeft = MAXRETRANS;
  this->packetNumber++;
  this->txSize = BLOCKSIZE;
  this->state = XMODEM_STATE_R_SYNC;
}

void XmodemCrc::r_sync() {
  Serial.println("r_sync()");
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

  Serial.print("this->tryChar = ");
  Serial.println(this->tryChar);

  if ((c = _inbyte((DLY_1S) << 1)) >= 0) {
    switch (c) {
      case CODE_SOH:
        this->state = XMODEM_STATE_R_FRAME;
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
        if ((c = _inbyte(DLY_1S)) == CODE_CAN) {
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

int jjz = 999;
void XmodemCrc::r_frame() {
  Serial.print("r_frame() ");
  Serial.println(this->packetNumber);

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

  int packetNumber = _inbyte(DLY_1S);
  int complimentPacketNumber = _inbyte(DLY_1S);

  if (packetNumber == -1) isRejected = true;
  if (complimentPacketNumber == -1) isRejected = true;
  Serial.print("receive checkchars: ");
  Serial.print("(this->pos) ");
  Serial.println(this->pos);

  for (int i = 0; i < this->txSize; i++)
  {
    size_t p = this->pos + i;
    int ch = _inbyte(DLY_1S);

    Serial.print("\t[");
    Serial.print(p);
    Serial.print("]");
    Serial.print("\t");
    Serial.print("ch = \t");
    Serial.print(ch);
    Serial.print("\t");
    Serial.write(ch);
    Serial.println("");

    if (ch == -1) {
      isRejected = true;
      break;
    }

    if (p <= this->bufSize) this->buf[p] = ch;
    calcRunningChecksum(ch);
  }
  Serial.println("");

  if (this->useCrc) {
    int crcHigh =  _inbyte(DLY_1S);
    int crcLow =  _inbyte(DLY_1S);

    int crc = (crcHigh * 256) + crcLow;

    Serial.println("\n\ncrc validate");
    Serial.print(crc, HEX);
    Serial.print(" ");
    Serial.println(this->crc, HEX);

    if (crc != this->crc) isRejected = true;
  } else {
    int ccks = _inbyte(DLY_1S);
    if (ccks != this->ccks) isRejected = true;
  }

  if (isRejected) {
    Serial.print("REJECTED");
    this->packetNumber--; // init increments it.
    this->r_init_frame();
  } else {
    Serial.print("Using crc? ");
    Serial.print(this->useCrc);
    Serial.println(" Sending ACK");
    _outbyte(CODE_ACK);
    this->r_init_frame();
    this->pos += this->txSize;
  }

  jjz--;
  if (!jjz) while (true);
}


///////////////////////////////////////////////////////////
void XmodemCrc::transmit(IoLine *_serial, unsigned char *src, int srcSize) {
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
    return;
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
  Serial.write(ch);

}

void XmodemCrc::t_init_frame() {
  this->crc = 0;
  this->ccks = 0;
  this->triesLeft = MAXRETRANS;
  this->packetNumber++;
  this->txSize = BLOCKSIZE;
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

  Serial.print("sending checkchars: ");
  for (int i = 0; i < this->txSize; i++)
  {
    size_t p = this->pos + i;

    unsigned char ch = (p <= this->bufSize) ? this->buf[p] : CODE_CTRLZ;
    calcRunningChecksum(ch);
    _outbyte(ch);
  }

  Serial.println("");

  if (this->useCrc) {
    _outbyte((this->crc >> 8) & 0xFF);
    _outbyte(this->crc & 0xFF);

    Serial.print("OUTGOING CRC: ");
    Serial.println(this->crc, HEX);
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

void XmodemCrc::t_eot() {
  signed int c;

  for (int retry = 0; retry < MAXRETRANS; ++retry) {
    _outbyte(CODE_EOT);
    if ((c = _inbyte((DLY_1S) << 1)) == CODE_ACK) break;
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

unsigned char XmodemCrc::getPacketNumber() {
  return this->packetNumber;
}

/* Good transmission strings and the CRC values.
  ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVW
  C34
  XYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJK
  37AD
  LMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ01234567
  298D
  0ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUV
  30F0
  WXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJ
  F2B1
  KLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456
  AAE8
  70ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTU
  BC0C
  VWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHI
  F432
  JKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345
  F0B6
  670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRST
  56DA
  UVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGH
  BD86
  IJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ01234
  C381
  5670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRS
  E7CC
  TUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFG
  191C
  HIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ0123
  8A4D
  45670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQRSTUVWXYZ012345670ABCDEFGHIJKLMNOPQR
  A992
*/
