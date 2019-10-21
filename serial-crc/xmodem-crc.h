#ifndef XMODEM_CRC_H
#define XMODEM_CRC_H

#include <stdint.h>
#include "ioline.h"

#define XMODEM_TIMEOUT (1000 * 1)
#define XMODEM_BLOCKSIZE 128


#define XMODEM_STATUS_RUNNING 0

#define XMODEM_STATE_DONE 0
#define XMODEM_STATE_T_INIT_TRANSMISSION 10
#define XMODEM_STATE_T_PACKET 11
#define XMODEM_STATE_T_EOT 12
#define XMODEM_STATE_R_SYNC 20
#define XMODEM_STATE_R_PACKET 21

class XmodemCrc {
  public:
    void transmit(IoLine *_serial, unsigned char *src, int srcSize);
    void receive(IoLine *_serial, unsigned char *dest, int destSize);

    unsigned char getPacketNumber();

    void next();
    bool isDone();
    unsigned char getState();
    signed char getStatus();

    unsigned short crc16_ccitt(unsigned short crc, unsigned char ch);

  private:
    void init_frame(char initMode);

    void t_init_transmission();
    void t_frame();
    void t_eot();

    void r_sync();
    void r_frame();


    void calcRunningChecksum(unsigned char ch);
    void _outbyte(int b);
    int _inbyte(int t);
    void flushinput(void);

    unsigned char state;
    signed char status;

    // DATA
    IoLine *serial;
    unsigned char *buf;
    int bufSize;
    unsigned char packetNumber;

    bool useCrc;
    unsigned char ccks; // Original XModem checksum
    unsigned short crc; // "Modern" CRC checksum
    int txSize;

    char tryChar;
    int pos; // Position in the buffer
    int triesLeft;
    int bytesInPacket;
};

#endif
