/*
 * Copyright 2001-2010 Georges Menie (www.menie.org)
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* this code needs standard functions memcpy() and memset()
   and input/output functions serial_read() and (*serial_write)().

   the prototypes of the input/output functions are:
     int serial_read(unsigned short timeout); // msec timeout
     void (*serial_write)(int c);

 */

#ifndef SERIAL_CRC_H
#define SERIAL_CRC_H
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
	unsigned char packetNumber;
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
	XMODEM_TRANSFER_STATUS receiveFullBuffer(unsigned char *dest, xmodem_t dest_size, void (*update_packet)(XModemPacketStatus status));
	XMODEM_TRANSFER_STATUS receiveCharacterMode(void (*put_char)(xmodem_t offset, xmodem_t i, unsigned char ch), void (*update_packet)(XModemPacketStatus status));
	XMODEM_TRANSFER_STATUS transmitFullBuffer(unsigned char *src, xmodem_t srch, void (*update_packet)(XModemPacketStatus status));
	XMODEM_TRANSFER_STATUS transmitCharacterMode(int (*get_char)(xmodem_t offset, xmodem_t i), void (*update_packet)(XModemPacketStatus status));

	void accumulateCrc(unsigned char ch);

private:
	unsigned char packetno;
	XMODEM_PACKET_ACTION packetAction;
	void updateStatus(XMODEM_PACKET_ACTION action, void (*update_packet)(XModemPacketStatus status));
	xmodem_t packetOffset;
	unsigned short packetCrc;
	unsigned char packetChecksome;
	bool useCrc = false;
	void (*serial_write)(int ch);
	int (*serial_read)(long int ms);
};
#endif
