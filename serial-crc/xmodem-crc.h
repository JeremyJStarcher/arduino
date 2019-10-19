#ifndef XMODEM_CRC_H
#define XMODEM_CRC_H

#include <stdint.h>
#include "ioline.h"

#define XMODEM_STATUS_RUNNING 0

//#define XMODEM_STATE_RUNNING 0
#define XMODEM_STATE_T_SYNC 1
#define XMODEM_STATE_T_FRAME 2
#define XMODEM_STATE_DONE 100


class XmodemCrc {
  public:
    void transmit(IoLine *_serial, unsigned char *src, int srcSize);
    void next();
    unsigned char status;
    unsigned short crc16_ccitt(unsigned short crc, unsigned char ch);

  private:
    void XmodemCrc::t_sync();
    void XmodemCrc::t_frame();

    void calcRunningChecksum(unsigned char ch);
    void _outbyte(int b);
    int _inbyte(int t);
    void flushinput(void);

    // DATA
    IoLine *serial;
    unsigned char *buf;
    int bufSize;
    unsigned char state;
    unsigned char packetNumber;
    bool useCrc;
    unsigned char ccks; // Original XModem checksum
    unsigned short crc; // "Modern" CRC checksum
    int txSize;

    int pos; // Position in the buffer
    int retries;
    int len;

    int bytesInPacket;
};

#endif
