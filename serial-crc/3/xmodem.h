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

#ifndef XMODEM_CRC_FAST
#ifndef XMODEM_CRC_SLOW
#define XMODEM_CRC_SLOW 1
#endif
#endif

#define XMODEM_SOH 0x01
#define XMODEM_STX 0x02
#define XMODEM_EOT 0x04
#define XMODEM_ACK 0x06
#define XMODEM_NAK 0x15
#define XMODEM_CAN 0x18
#define XMODEM_CTRLZ 0x1A

#define DELAY_1000 (1000)
#define DELAY_LONG (1000 * 10)
#define DELAY_1500 (1500)
#define DELAY_2000 (2000)

#define MAXRETRANS 25

unsigned char *xmodemBuffer;
size_t xmodemBuffer_size;

int getCharFromFullBuffer(xmodem_t offset, xmodem_t i)
{
	xmodem_t pos = offset + i;
	if (pos < xmodemBuffer_size)
	{
		return xmodemBuffer[pos];
	}
	else
	{
		return -1;
	}
}

int putCharInFullBuffer(xmodem_t offset, xmodem_t i, unsigned char ch)
{
	xmodem_t pos = offset + i;
	if (pos < xmodemBuffer_size)
	{
		xmodemBuffer[pos] = ch;
	}
}

static void flushinput(int (*serial_read)(long int ms))
{
	unsigned int cnt = 0;
	while (serial_read(DELAY_1500) >= 0)
		cnt++;
}

Xmodem::Xmodem(
	int (*_serial_read)(long int ms),
	void (*_serial_write)(int ch))
{
	this->serial_read = _serial_read;
	this->serial_write = _serial_write;
}

#if XMODEM_CRC_FAST
static const unsigned short crc16tab[256] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
	0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
	0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
	0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
	0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
	0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
	0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
	0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
	0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
	0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
	0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
	0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
	0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
	0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0};

void Xmodem::accumulateCrc(unsigned char ch)
{
	unsigned short crc = this->packetCrc;
	crc = (crc << 8) ^ crc16tab[((crc >> 8) ^ ch) & 0x00FF];
	this->packetCrc = crc;
}
#endif

#if XMODEM_CRC_SLOW
void Xmodem::accumulateCrc(unsigned char ch)
{
	this->packetCrc ^= ch << 8;
	for (char i = 0; i < 8; ++i)
	{
		if (this->packetCrc & 0x8000)
			this->packetCrc = (this->packetCrc << 1) ^ 0x1021;
		else
			this->packetCrc = this->packetCrc << 1;
	}

	this->packetChecksome += ch;
}
#endif

void Xmodem::updateStatus(XMODEM_PACKET_ACTION action, void (*update_packet)(XModemPacketStatus status))
{
	XModemPacketStatus status = XModemPacketStatus();
	status.packetNumber = this->packetno;
	status.action = action;
	(*update_packet)(status);
}

XMODEM_TRANSFER_STATUS Xmodem::receiveFullBuffer(
	unsigned char *dest,
	xmodem_t dest_size,
	void (*update_packet)(XModemPacketStatus status))
{
	xmodemBuffer = dest;
	xmodemBuffer_size = dest_size;

	return receiveCharacterMode(putCharInFullBuffer, update_packet);
}

XMODEM_TRANSFER_STATUS Xmodem::receiveCharacterMode(
	void (*put_char)(xmodem_t offset, xmodem_t i, unsigned char ch),
	void (*update_packet)(XModemPacketStatus status))
{
	int buffer_size;
	unsigned char trychar = 'C';
	int i, c;
	int retry, retrans = MAXRETRANS;
	this->packetCrc = 0;
	this->packetOffset = 0;
	this->packetno = 0;

	for (;;)
	{
		this->packetAction = XMODEM_PACKET_ACTION::Sync;
		this->updateStatus(this->packetAction, update_packet);

		for (retry = 0; retry < 16; ++retry)
		{
			if (trychar)
			{
				(*serial_write)(trychar);
			}

			if ((c = serial_read(DELAY_2000)) >= 0)
			{
				switch (c)
				{
				case XMODEM_SOH:
					buffer_size = 128;
					goto start_recv;
				case XMODEM_STX:
					buffer_size = 1024;
					goto start_recv;
				case XMODEM_EOT:
					flushinput(serial_read);
					(*serial_write)(XMODEM_ACK);
					return XMODEM_TRANSFER_STATUS::SUCCESS;
				case XMODEM_CAN:
					if ((c = serial_read(DELAY_1000)) == XMODEM_CAN)
					{
						flushinput(serial_read);
						(*serial_write)(XMODEM_ACK);
						return XMODEM_TRANSFER_STATUS::CANCELED_BY_REMOTE;
					}
					break;
				default:
					break;
				}
			}
		}
		if (trychar == 'C')
		{
			trychar = XMODEM_NAK;
			continue;
		}

		flushinput(serial_read);
		(*serial_write)(XMODEM_CAN);
		(*serial_write)(XMODEM_CAN);
		(*serial_write)(XMODEM_CAN);
		this->packetAction = XMODEM_PACKET_ACTION::SyncError;
		this->updateStatus(this->packetAction, update_packet);
		return XMODEM_TRANSFER_STATUS::SYNC_ERROR;

	start_recv:
		this->packetAction = XMODEM_PACKET_ACTION::Receiving;
		this->updateStatus(this->packetAction, update_packet);

		if (trychar == 'C')
		{
			this->useCrc = true;
		}

		trychar = 0;

		this->packetChecksome = 0;
		this->packetCrc = 0;

		unsigned char incomingPacketNumber;
		unsigned char incomingPacketNumber2;
		unsigned char incomingCrcHigh;
		unsigned char incomingCrcLow;

		unsigned char incomingChecksome;

		incomingPacketNumber = serial_read(DELAY_1000);
		if (c == -1)
		{
			this->packetAction = XMODEM_PACKET_ACTION::Timeout;
			goto reject;
		}

		incomingPacketNumber2 = serial_read(DELAY_1000);
		if (c == -1)
		{
			this->packetAction = XMODEM_PACKET_ACTION::Timeout;
			goto reject;
		}

		for (i = 0; i < buffer_size; ++i)
		{
			if ((c = serial_read(DELAY_1000)) < 0)
			{
				this->packetAction = XMODEM_PACKET_ACTION::Timeout;
				goto reject;
			}
			this->accumulateCrc(c);
			(*put_char)(this->packetOffset, i, c);
		}

		if (this->useCrc)
		{
			incomingCrcHigh = serial_read(DELAY_1000);
			if (c == -1)
			{
				this->packetAction = XMODEM_PACKET_ACTION::Timeout;
				goto reject;
			}
			incomingCrcLow = serial_read(DELAY_1000);
			if (c == -1)
			{
				this->packetAction = XMODEM_PACKET_ACTION::Timeout;
				goto reject;
			}
		}
		else
		{
			incomingChecksome = serial_read(DELAY_1000);
			if (c == -1)
			{
				this->packetAction = XMODEM_PACKET_ACTION::Timeout;
				goto reject;
			}
		}

		if (incomingPacketNumber != (unsigned char)(~incomingPacketNumber2))
		{
			this->packetAction = XMODEM_PACKET_ACTION::PacketNumberCorrupt;
			goto reject;
		}

		if (!(incomingPacketNumber == packetno || incomingPacketNumber == (unsigned char)packetno - 1))
		{
			this->packetAction = XMODEM_PACKET_ACTION::PacketNumberOutOfSequence;
			goto reject;
		}

		if (this->useCrc)
		{
			unsigned short crc = (incomingCrcHigh * 256) + incomingCrcLow;
			if (crc != this->packetCrc)
			{
				this->packetAction = XMODEM_PACKET_ACTION::CrcMismatch;
				goto reject;
			}
		}
		else
		{
			if (incomingChecksome != this->packetChecksome)
			{
				this->packetAction = XMODEM_PACKET_ACTION::ChecksomeMismatch;
				goto reject;
			}
		}


		if (incomingPacketNumber == packetno)
		{
			this->packetAction = XMODEM_PACKET_ACTION::Accepted;
			this->updateStatus(this->packetAction, update_packet);
			this->packetOffset += buffer_size;
			++packetno;
			retrans = MAXRETRANS + 1;
		}
		else
		{
			this->packetAction = XMODEM_PACKET_ACTION::ValidDuplicate;
			this->updateStatus(this->packetAction, update_packet);
		}

		if (--retrans <= 0)
		{
			flushinput(serial_read);
			(*serial_write)(XMODEM_CAN);
			(*serial_write)(XMODEM_CAN);
			(*serial_write)(XMODEM_CAN);
			return XMODEM_TRANSFER_STATUS::TOO_MANY_RETRIES;
		}

		(*serial_write)(XMODEM_ACK);
		continue;

	reject:
		this->updateStatus(this->packetAction, update_packet);
		flushinput(serial_read);
		(*serial_write)(XMODEM_NAK);
	}
}

XMODEM_TRANSFER_STATUS Xmodem::transmitFullBuffer(
	unsigned char *src, xmodem_t source_size,
	void (*update_packet)(XModemPacketStatus status))
{
	xmodemBuffer_size = source_size;
	xmodemBuffer = src;
	return this->transmitCharacterMode(getCharFromFullBuffer, update_packet);
}

XMODEM_TRANSFER_STATUS Xmodem::transmitCharacterMode(
	int (*get_char)(xmodem_t offset, xmodem_t i),
	void (*update_packet)(XModemPacketStatus status))
{
	int buffer_size;
	int i, c;
	int retry;
	bool is_eof = false;
	this->packetOffset = 0;
	this->packetno = 0;

	for (;;)
	{
		for (retry = 0; retry < 16; ++retry)
		{
			this->packetAction = XMODEM_PACKET_ACTION::WaitingForReceiver;
			this->updateStatus(this->packetAction, update_packet);
			if ((c = serial_read(DELAY_1500)) < 0)
			{
				// Logging was here.
			}
			else
			{
				switch (c)
				{
				case 'C':
					this->useCrc = true;
					goto start_trans;
				case XMODEM_NAK:
					this->useCrc = false;
					goto start_trans;
				case XMODEM_CAN:
					if ((c = serial_read(DELAY_1000)) == XMODEM_CAN)
					{
						(*serial_write)(XMODEM_ACK);
						flushinput(serial_read);
						return XMODEM_TRANSFER_STATUS::CANCELED_BY_REMOTE;
					}
					break;
				default:
					break;
				}
			}
		}

		(*serial_write)(XMODEM_CAN);
		(*serial_write)(XMODEM_CAN);
		(*serial_write)(XMODEM_CAN);
		flushinput(serial_read);
		return XMODEM_TRANSFER_STATUS::SYNC_ERROR;

		for (;;)
		{
		start_trans:
			this->packetAction = XMODEM_PACKET_ACTION::Sync;
			this->updateStatus(this->packetAction, update_packet);

			buffer_size = 128;

			if (!is_eof)
			{
				for (retry = 0; retry < MAXRETRANS; ++retry)
				{
					this->packetAction = XMODEM_PACKET_ACTION::Transmitting;
					this->updateStatus(this->packetAction, update_packet);

					(*serial_write)(XMODEM_SOH);
					(*serial_write)(packetno);
					(*serial_write)((unsigned char)(~packetno));

					this->packetChecksome = 0;
					this->packetCrc = 0;

					for (i = 0; i < buffer_size; ++i)
					{
						int khar = (*get_char)(this->packetOffset, i);
						if (khar == -1)
						{
							is_eof = true;
						}

						unsigned char ch = (khar == -1) ? (XMODEM_CTRLZ) : (unsigned char)khar;

						(*serial_write)(ch);
						this->accumulateCrc(ch);
					}

					if (this->useCrc)
					{
						(*serial_write)((this->packetCrc & 0xFF00) >> 8);
						(*serial_write)(this->packetCrc & 0xFF);
					}
					else
					{
						(*serial_write)(this->packetChecksome);
					}

					if ((c = serial_read(DELAY_LONG)) >= 0)
					{
						switch (c)
						{
						case XMODEM_ACK:
							this->packetAction = XMODEM_PACKET_ACTION::ReceiverACK;
							this->updateStatus(this->packetAction, update_packet);
							++packetno;
							this->packetOffset += buffer_size;
							goto start_trans;
						case XMODEM_CAN:
							if ((c = serial_read(DELAY_1000)) == XMODEM_CAN)
							{
								(*serial_write)(XMODEM_ACK);
								flushinput(serial_read);
								return XMODEM_TRANSFER_STATUS::CANCELED_BY_REMOTE;
							}
							break;
						case XMODEM_NAK:
							this->packetAction = XMODEM_PACKET_ACTION::ReceiverNAK;
							this->updateStatus(this->packetAction, update_packet);
							break;
						default:
							this->packetAction = XMODEM_PACKET_ACTION::ReceiverGarbage;
							this->updateStatus(this->packetAction, update_packet);
							break;
						}
					}
				}

				(*serial_write)(XMODEM_CAN);
				(*serial_write)(XMODEM_CAN);
				(*serial_write)(XMODEM_CAN);
				flushinput(serial_read);
				return XMODEM_TRANSFER_STATUS::TRANSMIT_ERROR;
			}
			else
			{
				for (retry = 0; retry < 10; ++retry)
				{
					(*serial_write)(XMODEM_EOT);
					if ((c = serial_read(DELAY_2000)) == XMODEM_ACK)
					{
						break;
					}
				}
				flushinput(serial_read);
				return (c == XMODEM_ACK) ? XMODEM_TRANSFER_STATUS::SUCCESS : XMODEM_TRANSFER_STATUS::NO_EOT_REPLY;
			}
		}
	}
}

#endif
