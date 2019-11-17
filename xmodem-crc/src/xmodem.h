#ifndef SERIAL_CRC_H
#define SERIAL_CRC_H

#ifndef NO_ARDUINO
#include <Arduino.h>
#endif

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
using namespace std;

#define XMODEM_SOH 0x01
#define XMODEM_STX 0x02
#define XMODEM_EOT 0x04
#define XMODEM_ACK 0x06
#define XMODEM_NAK 0x15
#define XMODEM_CAN 0x18
#define XMODEM_CTRLZ 0x1A

/* The XMODEM_INIT_STATE is only sent with the very first packet
   and is used to decide if the transfer is in CRC or checksome
   mode.  Once that has been established, there is no need to send
   it again. */
enum class XMODEM_INIT_STATE : unsigned char
{
	ATTEMPTED_CRC = 'C',
	ATTEMPT_CHECKSOME = XMODEM_NAK,
	RESOLVED = 0,
};

enum class XMODEM_PACKET_ACTION : signed char
{
	Receiving = 1,
	Timeout = 2,
	PacketNumberCorrupt = 3,
	PacketNumberOutOfSequence = 4,
	CrcMismatch = 5,
	ChecksomeMismatch = 6,
	Accepted = 7,
	ValidDuplicate = 8,
	ReceiverACK = 9,
	ReceiverNAK = 10,
	ReceiverGarbage = 11,
	Sync = 12,
	SyncError = 13,
	Transmitting = 14,
	WaitingForReceiver = 15,
};

class XModemPacketStatus
{
public:
	XMODEM_PACKET_ACTION action;
	unsigned int packetNumber;
};

enum class XMODEM_TRANSFER_STATUS : signed char
{
	SUCCESS = 0,
	CANCELED_BY_REMOTE = -1,
	SYNC_ERROR = -2,
	TOO_MANY_RETRIES = -3,
	TRANSMIT_ERROR = -4,
	NO_EOT_REPLY = -5,
};

typedef unsigned long xmodem_t;
class Xmodem
{
public:
	Xmodem(int (*serial_read)(long int ms), void (*serial_write)(int ch));
#ifndef NO_ARDUINO
	Xmodem(Stream *S);
#endif
	XMODEM_TRANSFER_STATUS receiveFullBuffer(unsigned char *dest, xmodem_t dest_size, void (*broadcastPacketChange)(XModemPacketStatus status));
	XMODEM_TRANSFER_STATUS receiveCharacterMode(void (*storeCharacter)(xmodem_t offset, xmodem_t i, unsigned char ch), void (*updatePacketData)(XModemPacketStatus status));
	XMODEM_TRANSFER_STATUS transmitFullBuffer(unsigned char *src, xmodem_t srch, void (*updatePacketData)(XModemPacketStatus status));
	XMODEM_TRANSFER_STATUS transmitCharacterMode(int (*retrieveCharacter)(xmodem_t offset, xmodem_t i), void (*updatePacketData)(XModemPacketStatus status));

	void accumulateCrc(unsigned char ch);

private:
	// the full packet number. Does not wrap around if packet# is > 255. Starts at 0
	unsigned int fullPacketNumber;

	// Starts at 1. Wraps around.
	unsigned char packetno;
	XMODEM_PACKET_ACTION packetAction;
	void updateStatus(XMODEM_PACKET_ACTION action, void (*updatePacketData)(XModemPacketStatus status));
	xmodem_t packetOffset;
	unsigned short packetCrc;
	unsigned char packetChecksome;
	bool useCrc = false;
#ifndef NO_ARDUINO
	Stream *stream;
	bool hasStreamObject;
#endif
	int streamRead(long int ms);
	void streamWrite(int ch);
	void (*serial_write)(int ch);
	int (*serial_read)(long int ms);
	void flushinput();
	void putChar(int ch);
	int getChar(long int ms);
};
#endif
