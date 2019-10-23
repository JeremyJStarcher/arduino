#ifndef XMODEM_CRC_H
#define XMODEM_CRC_H

#include <stdint.h>
#include "ioline.h"

#define XMODEM_TIMEOUT (1000 * 1)

#define XMODEM_STATUS_RUNNING 0


extern int XMODEM_SEND_EXTRA_CHARS;
extern int XMODEM_SEND_EAT_CHARS;
extern int XMODEM_BREAK_CRC;


#define XMODEM_STATE_DONE 0

#define XMODEM_STATE_T_INIT_TRANSMISSION 10
#define XMODEM_STATE_T_PACKET 11
#define XMODEM_STATE_T_WAIT_REPLY 13
#define XMODEM_STATE_T_EOT 14

#define XMODEM_STATE_R_SYNC 20
#define XMODEM_STATE_R_PACKET 21

class XmodemCrc {
  public:
    void transmit(IoLine *_serial, size_t packetSize);
    void receive(IoLine *_serial, size_t packetSize);

    unsigned char getPacketNumber();

    // If bytes < packetSize then it ends the transmission
    // if bytes > packetSize then it panics.
    void nextTransmit(char *buf, size_t bytes);
    void nextRecieve(char *buf, size_t bytes);

    bool isDone();
    unsigned char getState();
    signed char getStatus();

    unsigned short crc16_ccitt(unsigned short crc, unsigned char ch);

    bool hasData;
  private:
    void init_frame(char initMode);

    void t_init_transmission();
    void t_frame(char *buf, size_t  bytes);
    void t_waitReply(size_t bytes);
    void t_eot();

    void r_sync();
    void r_frame(char *buf, size_t bytes);

    void calcRunningChecksum(unsigned char ch);
    void _outbyte(int b);
    int _inbyte(int t);
    int flushinput(void);

    unsigned char state;
    signed char status;

    // DATA
    IoLine *serial;
    int bufSize;
    unsigned char packetNumber;

    bool useCrc;
    unsigned char ccks; // Original XModem checksum
    unsigned short crc; // "Modern" CRC checksum

    bool needNextPacket;
    char tryChar;
    int triesLeft;
};

#endif
