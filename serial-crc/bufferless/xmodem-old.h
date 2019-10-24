#include "xmodem-crc.h"
#define OLD_XMODEM_BLOCK 128
#define DLY_1S (XMODEM_TIMEOUT * 10)

#define LOGNL(x) Serial.println(x)
#define LOG(x) Serial.print(x)

/*
   Copyright 2001-2010 Georges Menie (www.menie.org)
   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

 *     * Redistributions of source code must retain the above copyright
         notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
         notice, this list of conditions and the following disclaimer in the
         documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
         names of its contributors may be used to endorse or promote products
         derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
   EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* this code needs standard functions memcpy() and memset()
   and input/output functions _inbyte(true, ) and _outbyte(true, ).

   the prototypes of the input/output functions are:
     int _inbyte(true, unsigned short timeout); // msec timeout
     void _outbyte(true, int c);

*/

#include <stdint.h>
#include "ioline.h"

#define XMODEM_SOH  0x01
#define XMODEM_STX  0x02
#define XMODEM_EOT  0x04
#define XMODEM_ACK  0x06
#define XMODEM_NAK  0x15
#define XMODEM_CAN  0x18
#define XMODEM_CTRLZ 0x1A

#define MAXRETRANS 25

bool zusedCrc = false;

class XmodemOld {
  public:
    void begin(IoLine *_serial);
    int xmodemReceive(unsigned char *dest, int destsz);
    int xmodemTransmit(unsigned char *src, int srcsz);
    //int xmodemTransmit(unsigned char *src, int srcsz, int tmpsz, void (*getsrc)());
    bool usedCrc();
  private:
    IoLine *serial;
    void _outbyte(bool doLog, int b);
    int _inbyte(bool doLog, int t);
    void flushinput(void);
    static int check(int crc, const unsigned char *buf, int sz);
};

bool XmodemOld::usedCrc() {
  return zusedCrc;
}

void XmodemOld::begin(IoLine *_serial) {
  this->serial = _serial;
}

int XmodemOld::xmodemReceive(unsigned char *dest, int destsz)
{
  unsigned char xbuff[1030]; /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */
  unsigned char *p;
  int bufsz, crc = 0;
  unsigned char trychar = 'C';
  unsigned char packetno = 1;
  int i, c, len = 0;
  int retry, retrans = MAXRETRANS;

  for (;;) {
    for ( retry = 0; retry < 16; ++retry) {
      if (trychar) _outbyte(true, trychar);
      if ((c = _inbyte(true, (DLY_1S) << 1)) >= 0) {
        switch (c) {
          case XMODEM_SOH:
            bufsz = OLD_XMODEM_BLOCK;
            goto start_recv;
          case XMODEM_STX:
            bufsz = 1024;
            goto start_recv;
          case XMODEM_EOT:
            flushinput();
            _outbyte(true, XMODEM_ACK);
            return len; /* normal end */
          case XMODEM_CAN:
            if ((c = _inbyte(true, DLY_1S)) == XMODEM_CAN) {
              flushinput();
              _outbyte(true, XMODEM_ACK);
              return -1; /* canceled by remote */
            }
            break;
          default:
            break;
        }
      }
    }
    if (trychar == 'C') {
      trychar = XMODEM_NAK;
      continue;
    }
    flushinput();
    _outbyte(true, XMODEM_CAN);
    _outbyte(true, XMODEM_CAN);
    _outbyte(true, XMODEM_CAN);
    return -2; /* sync error */

start_recv:
    if (trychar == 'C') crc = 1;
    trychar = 0;
    p = xbuff;
    *p++ = c;
    LOG("<in buffer>");
    for (i = 0;  i < (bufsz + (crc ? 1 : 0) + 3); ++i) {
      if ((c = _inbyte(false, DLY_1S)) < 0) goto reject;
      LOG(c);
      LOG(" ");
      *p++ = c;
    }
    LOGNL("");

    if (xbuff[1] == (unsigned char)(~xbuff[2]) &&
        (xbuff[1] == packetno || xbuff[1] == (unsigned char)packetno - 1) &&
        check(crc, &xbuff[3], bufsz)) {
      if (xbuff[1] == packetno) {
        register int count = destsz - len;
        if (count > bufsz) count = bufsz;
        if (count > 0) {
          memcpy (&dest[len], &xbuff[3], count);
          len += count;
        }
        ++packetno;
        retrans = MAXRETRANS + 1;
      }
      if (--retrans <= 0) {
        flushinput();
        _outbyte(true, XMODEM_CAN);
        _outbyte(true, XMODEM_CAN);
        _outbyte(true, XMODEM_CAN);
        return -3; /* too many retry error */
      }
      _outbyte(true, XMODEM_ACK);
      continue;
    }
reject:
    flushinput();
    _outbyte(true, XMODEM_NAK);
  }
}


int XmodemOld::_inbyte(bool doLog, int t) {
  int ch = this->serial->readbyte(t);
  if (doLog) {
    LOG("<in>\t"); LOGNL(ch);
  }
  return ch;
}

void XmodemOld::_outbyte(bool doLog, int b) {
  if (doLog) {
    LOG("<out>\t"); LOGNL(b);
  }
  this->serial->writebyte(b);
}

unsigned short crc16_ccitt(const void *buf, int len)
{
  zusedCrc = true;
  unsigned short crc = 0;
  while ( len-- ) {
    int i;

    crc ^= *(char *)buf++ << 8;
    for ( i = 0; i < 8; ++i ) {
      if ( crc & 0x8000 )
        crc = (crc << 1) ^ 0x1021;
      else
        crc = crc << 1;
    }
  }
  return crc;
}

static int XmodemOld::check(int crc, const unsigned char *buf, int sz)
{
  if (crc) {
    unsigned short crc = crc16_ccitt(buf, sz);
    unsigned short tcrc = (buf[sz] << 8) + buf[sz + 1];
    if (crc == tcrc)
      return 1;
  }
  else {
    int i;
    unsigned char cks = 0;
    for (i = 0; i < sz; ++i) {
      cks += buf[i];
    }
    if (cks == buf[sz])
      return 1;
  }

  return 0;
}

void XmodemOld::flushinput(void)
{
  LOGNL("<flush input>");
  while (_inbyte(false, ((DLY_1S) * 3) >> 1) >= 0)
    ;
}

int XmodemOld::xmodemTransmit(
  unsigned char *src,
  int srcsz
)
{
  unsigned char xbuff[1030]; /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */
  int bufsz, crc = -1;
  unsigned char packetno = 1;
  int i, c, len = 0;
  int retry;

  for (;;) {
    for ( retry = 0; retry < 16; ++retry) {
      if ((c = _inbyte(true, (DLY_1S) << 1)) >= 0) {
        switch (c) {
          case 'C':
            crc = 1;
            goto start_trans;
          case XMODEM_NAK:
            crc = 0;
            goto start_trans;
          case XMODEM_CAN:
            if ((c = _inbyte(true, DLY_1S)) == XMODEM_CAN) {
              _outbyte(true, XMODEM_ACK);
              flushinput();
              return -1; /* canceled by remote */
            }
            break;
          default:
            break;
        }
      }
    }
    _outbyte(true, XMODEM_CAN);
    _outbyte(true, XMODEM_CAN);
    _outbyte(true, XMODEM_CAN);
    flushinput();
    return -2; /* no sync */

    for (;;) {
start_trans:
      xbuff[0] = XMODEM_SOH; bufsz = OLD_XMODEM_BLOCK;
      xbuff[1] = packetno;
      xbuff[2] = ~packetno;
      c = srcsz - len;
      if (c > bufsz) c = bufsz;
      if (c >= 0) {
        memset (&xbuff[3], 0, bufsz);

        if (c == 0) {
          xbuff[3] = XMODEM_CTRLZ;
        }
        else {
          memcpy (&xbuff[3], &src[len], c);
          if (c < bufsz) xbuff[3 + c] = XMODEM_CTRLZ;
        }

        if (crc) {
          unsigned short ccrc = crc16_ccitt(&xbuff[3], bufsz);
          xbuff[bufsz + 3] = (ccrc >> 8) & 0xFF;
          xbuff[bufsz + 4] = ccrc & 0xFF;
        }
        else {
          unsigned char ccks = 0;
          for (i = 3; i < bufsz + 3; ++i) {
            ccks += xbuff[i];
          }
          xbuff[bufsz + 3] = ccks;
        }
        LOG("<outbuffer>");
        for (retry = 0; retry < MAXRETRANS; ++retry) {
          int bytesInPacket = bufsz + 4 + (crc ? 1 : 0);
          for (i = 0; i < bytesInPacket; ++i) {
            _outbyte(false, xbuff[i]);
            LOG(xbuff[i]);
            LOG(" ");
          }
          LOGNL("");

          if ((c = _inbyte(true, DLY_1S)) >= 0 ) {
            switch (c) {
              case XMODEM_ACK:
                ++packetno;
                len += bufsz;
                goto start_trans;
              case XMODEM_CAN:
                if ((c = _inbyte(true, DLY_1S)) == XMODEM_CAN) {
                  _outbyte(true, XMODEM_ACK);
                  flushinput();
                  return -1; /* canceled by remote */
                }
                break;
              case XMODEM_NAK:
              default:
                break;
            }
          }
        }
        _outbyte(true, XMODEM_CAN);
        _outbyte(true, XMODEM_CAN);
        _outbyte(true, XMODEM_CAN);
        flushinput();
        return -4; /* xmit error */
      }

      else {
        for (retry = 0; retry < 10; ++retry) {
          _outbyte(true, XMODEM_EOT);
          if ((c = _inbyte(true, (DLY_1S) << 1)) == XMODEM_ACK) break;
        }
        flushinput();
        return (c == XMODEM_ACK) ? len : -5;
      }
    }
  }
}

#ifdef TEST_XMODEM_RECEIVE
int main(void)
{
  int st;

  printf ("Send data using the xmodem protocol from your terminal emulator now...\n");
  /* the following should be changed for your environment:
     0x30000 is the download address,
     65536 is the maximum size to be written at this address
  */
  st = xmodemReceive((char *)0x30000, 65536);
  if (st < 0) {
    printf ("Xmodem receive error: status: %d\n", st);
  }
  else  {
    printf ("Xmodem successfully received %d bytes\n", st);
  }

  return 0;
}
#endif
#ifdef TEST_XMODEM_SEND
int main(void)
{
  int st;

  printf ("Prepare your terminal emulator to receive data now...\n");
  /* the following should be changed for your environment:
     0x30000 is the download address,
     12000 is the maximum size to be send from this address
  */
  st = xmodemTransmit((char *)0x30000, 12000);
  if (st < 0) {
    printf ("Xmodem transmit error: status: %d\n", st);
  }
  else  {
    printf ("Xmodem successfully transmitted %d bytes\n", st);
  }

  return 0;
}
#endif
