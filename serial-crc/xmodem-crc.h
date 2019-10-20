#ifndef XMODEM_CRC_H
#define XMODEM_CRC_H

#include <stdint.h>
#include "ioline.h"

#define XMODEM_STATUS_RUNNING 0

#define XMODEM_STATE_DONE 0
#define XMODEM_STATE_T_SYNC 10
#define XMODEM_STATE_T_FRAME 11
#define XMODEM_STATE_T_EOT 12
#define XMODEM_STATE_R_SYNC 20

class XmodemCrc {
  public:
    void transmit(IoLine *_serial, unsigned char *src, int srcSize);
    void receive(IoLine *_serial, unsigned char *dest, int destSize);

    unsigned char getPacketNumber();

    void next();
    bool isDone();
    signed char status;
    unsigned short crc16_ccitt(unsigned short crc, unsigned char ch);

  private:
    void t_sync();
    void t_frame();
    void t_init_frame();
    void t_eot();

    void r_sync();
    void r_init_frame();

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
