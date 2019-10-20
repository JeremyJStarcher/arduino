#ifndef XMODEM_CRC_H
#define XMODEM_CRC_H

#include <stdint.h>
#include "ioline.h"

#define XMODEM_STATUS_RUNNING 0

#define XMODEM_STATE_DONE 0
#define XMODEM_STATE_T_SYNC 1
#define XMODEM_STATE_T_FRAME 2
#define XMODEM_STATE_T_EOT 3

class XmodemCrc {
  public:
    void transmit(IoLine *_serial, unsigned char *src, int srcSize);
    void next();
    bool isDone();
    unsigned char status;
    unsigned short crc16_ccitt(unsigned short crc, unsigned char ch);

  private:
    void t_sync();
    void t_frame();
    void t_init_frame();
    void t_eot();

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
    int triesLeft;
    int bytesInPacket;
};

#endif
