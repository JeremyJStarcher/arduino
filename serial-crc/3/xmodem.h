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

enum class XMODEM_ERRORS : signed char
{
	NONE = 0,
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
	XMODEM_ERRORS receiveFullBuffer(unsigned char *dest, xmodem_t destsz);
	XMODEM_ERRORS receiveCharacterMode(void (*put_char)(xmodem_t offset, xmodem_t i, unsigned char ch));
	XMODEM_ERRORS transmitFullBuffer(unsigned char *src, xmodem_t srcsz);
	XMODEM_ERRORS transmitCharacterMode(int (*get_char)(xmodem_t offset, xmodem_t i));

	void accumulateCrc(unsigned char ch);

private:
	xmodem_t packetOffset;
	unsigned short packetCrc;
	unsigned char packetChecksome;
	bool useCrc = false;
	void (*serial_write)(int ch);
	int (*serial_read)(long int ms);
};

#ifndef XMODEM_LOG_NULL
#ifndef XMODEM_LOG_IOSTREAM
#ifndef XMODEM_LOG_SERIAL0
#define XMODEM_LOG_NULL 1
#endif
#endif
#endif

unsigned char *xbuf;
size_t xbufsz;

#ifndef XMODEM_CRC_FAST
#ifndef XMODEM_CRC_SLOW
#define XMODEM_CRC_SLOW 1
#endif
#endif

#ifdef XMODEM_LOG_SERIAL0
#include <Arduino.h>

#define LOG(x)              \
	Serial.print("(LOG) "); \
	Serial.print(x)
#define LOGLN(x)     \
	Serial.print(x); \
	Serial.println("");

#endif

#ifdef XMODEM_LOG_NULL
#define LOG(x)
#define LOGLN(x)
#endif

#ifdef XMODEM_LOG_IOSTREAM

#include <iostream>

#define LOG(x) cout << x
#define LOGLN(x) \
	cout << x;   \
	cout << "\n"
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

int getCharFromBuf(xmodem_t offset, xmodem_t i)
{
	xmodem_t pos = offset + i;
	 if (pos < xbufsz)
	{
		return xbuf[pos];
	}
	else
	{
		return -1;
	}
}

int putCharInBuf(xmodem_t offset, xmodem_t i, unsigned char ch)
{
	xmodem_t pos = offset + i;
	if (pos < xbufsz)
	{
		xbuf[pos] = ch;
	}
}

static void flushinput(int (*serial_read)(long int ms))
{
	unsigned int cnt = 0;
	while (serial_read(DELAY_1500) >= 0)
		cnt++;
#if XMODEM_WRITE_LOG
	fprintf(logFile, "Flushinput (%d flushed)\n", cnt);
#endif
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

XMODEM_ERRORS Xmodem::receiveFullBuffer(
	unsigned char *dest,
	xmodem_t destsz)
{
	xbuf = dest;
	xbufsz = destsz;

	return receiveCharacterMode(putCharInBuf);
}

XMODEM_ERRORS Xmodem::receiveCharacterMode(
	void (*put_char)(xmodem_t offset, xmodem_t i, unsigned char ch))
{
	int bufsz;
	unsigned char trychar = 'C';
	unsigned char packetno = 1;
	int i, c;
	int retry, retrans = MAXRETRANS;
	this->packetCrc = 0;
	this->packetOffset = 0;

	for (;;)
	{
		for (retry = 0; retry < 16; ++retry)
		{
			if (trychar)
			{
#if XMODEM_WRITE_LOG
				fprintf(logFile, "CRC/Checksome start %d\n", trychar);
#endif
				(*serial_write)(trychar);
			}
			if ((c = serial_read(DELAY_2000)) >= 0)
			{
#if XMODEM_WRITE_LOG
				fprintf(logFile, "CRC/Checksome received: %d\n", c);
#endif
				switch (c)
				{
				case XMODEM_SOH:
					LOGLN("RECIEVED XMODEM_SOH (128 byte buffer start)");
					bufsz = 128;
					goto start_recv;
				case XMODEM_STX:
					LOGLN("RECIEVED XMODEM_STX (1024 byte buffer start)");
					bufsz = 1024;
					goto start_recv;
				case XMODEM_EOT:
					LOGLN("RECIEVED XMODEM_EOT");
					flushinput(serial_read);
					(*serial_write)(XMODEM_ACK);
					return XMODEM_ERRORS::NONE;
				case XMODEM_CAN:
					LOGLN("RECIEVED CAN");
					if ((c = serial_read(DELAY_1000)) == XMODEM_CAN)
					{
						flushinput(serial_read);
						(*serial_write)(XMODEM_ACK);
						return XMODEM_ERRORS::CANCELED_BY_REMOTE;
					}
					break;
				default:
					break;
				}
			}
		}
		if (trychar == 'C')
		{
#if XMODEM_WRITE_LOG
			fprintf(logFile, "Trying to start in checksome mode..\n");
#endif
			trychar = XMODEM_NAK;
			continue;
		}
#if XMODEM_WRITE_LOG
		fprintf(logFile, "Sync error\n");
#endif
		flushinput(serial_read);
		(*serial_write)(XMODEM_CAN);
		(*serial_write)(XMODEM_CAN);
		(*serial_write)(XMODEM_CAN);
		return XMODEM_ERRORS::SYNC_ERROR;

	start_recv:
		LOG("Receiving packet ");
		LOG((int)packetno);
		LOG(" retries ");
		LOGLN(retrans);
#if XMODEM_WRITE_LOG
		fprintf(logFile, "Start receive packet: %d\n", packetno);
#endif

		if (trychar == 'C')
		{
			useCrc = true;
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
			LOGLN("REJECT: incomingPacketNumber timeout");
			goto reject;
		}

		incomingPacketNumber2 = serial_read(DELAY_1000);
		if (c == -1)
		{
			LOGLN("REJECT: incomingPacketNumber2 timeout");
			goto reject;
		}

		for (i = 0; i < bufsz; ++i)
		{
#if XMODEM_WRITE_LOG
			fprintf(logFile, "receiving %d byte: %d\n", packetno, i);
#endif
			if ((c = serial_read(DELAY_1000)) < 0)
			{
				LOGLN("REJECT: payload timeout");
#if XMODEM_WRITE_LOG
				fprintf(logFile, "read timeout. rejecting\n");
#endif
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
				LOGLN("REJECT: crcHigh timeout");
				goto reject;
			}
			incomingCrcLow = serial_read(DELAY_1000);
			if (c == -1)
			{
				LOGLN("REJECT: crcLow timeout");
				goto reject;
			}
		}
		else
		{
			incomingChecksome = serial_read(DELAY_1000);
			if (c == -1)
			{
				LOGLN("REJECT: checksome timeout");
				goto reject;
			}
		}

#if XMODEM_WRITE_LOG
		fprintf(logFile, "Entire packet read\n");
#endif
		LOGLN("Read entire packet. Performing checks.");
		if (incomingPacketNumber != (unsigned char)(~incomingPacketNumber2))
		{
			LOGLN("REJECT: incomingPacketNumber ~incomingPacketNumber2 mismatch");
			goto reject;
		}

		if (!(incomingPacketNumber == packetno || incomingPacketNumber == (unsigned char)packetno - 1))
		{
			LOGLN("REJECT: incomingPacketNumber packetNumber mismatch");
			goto reject;
		}

		if (this->useCrc)
		{
			unsigned short crc = (incomingCrcHigh * 256) + incomingCrcLow;
			if (crc != this->packetCrc)
			{
				LOGLN("REJECT: packetCrc mismatch");
				goto reject;
			}
		}
		else
		{
			if (incomingChecksome != this->packetChecksome)
			{
				LOGLN("REJECT: packetChecksome mismatch");
				goto reject;
			}
		}

		LOGLN("PACKET PASSED ALL CHECKS");
#if XMODEM_WRITE_LOG
		fprintf(logFile, "Passed check #1\n");
#endif

		if (incomingPacketNumber == packetno)
		{
			LOGLN("ACCEPTING PACKET");
#if XMODEM_WRITE_LOG
			fprintf(logFile, "Passed check #2\n");
#endif
			this->packetOffset += bufsz;
			++packetno;
			retrans = MAXRETRANS + 1;
		}
#if XMODEM_WRITE_LOG
		fprintf(logFile, "Retries left %d\n", retrans);
#endif

		if (--retrans <= 0)
		{
			flushinput(serial_read);
			(*serial_write)(XMODEM_CAN);
			(*serial_write)(XMODEM_CAN);
			(*serial_write)(XMODEM_CAN);
			return XMODEM_ERRORS::TOO_MANY_RETRIES;
		}
#if XMODEM_WRITE_LOG
		fprintf(logFile, "Packet accepted\n");
#endif

		LOGLN("SENDING PACKET ACK");
		(*serial_write)(XMODEM_ACK);
		continue;

	reject:
#if XMODEM_WRITE_LOG
		fprintf(logFile, "Packet rejected\n");
#endif
		LOGLN("SENDING PACKET NAK");
		flushinput(serial_read);
		(*serial_write)(XMODEM_NAK);
	}
}

XMODEM_ERRORS Xmodem::transmitFullBuffer(unsigned char *src, xmodem_t srcsz)
{
	xbufsz = srcsz;
	xbuf = src;
	return this->transmitCharacterMode(getCharFromBuf);
}

XMODEM_ERRORS Xmodem::transmitCharacterMode(int (*get_char)(xmodem_t offset, xmodem_t i))
{
	int bufsz;
	unsigned char packetno = 1;
	int i, c;
	int retry;
	bool is_eof = false;
	this->packetOffset = 0;

	for (;;)
	{
#if XMODEM_WRITE_LOG
		fprintf(logFile, "Start of forever loop\n");
#endif
		for (retry = 0; retry < 16; ++retry)
		{
#if XMODEM_WRITE_LOG
			fprintf(logFile, "Start of retry loop\n");
#endif
			if ((c = serial_read(DELAY_1500)) < 0)
			{
#if XMODEM_WRITE_LOG
				fprintf(logFile, "No reply to packet %d\n", c);
#endif
			}
			else
			{
#if XMODEM_WRITE_LOG
				fprintf(logFile, "Transmit header received %d\n", c);
#endif
				switch (c)
				{
				case 'C':
					LOGLN("RECIEVER WANTS CRC");
					this->useCrc = true;
					goto start_trans;
				case XMODEM_NAK:
					LOGLN("RECIEVER WANTS CHECKSOME");
					this->useCrc = false;
					goto start_trans;
				case XMODEM_CAN:
					LOGLN("RECIEVER CANCELLED");
					if ((c = serial_read(DELAY_1000)) == XMODEM_CAN)
					{
						(*serial_write)(XMODEM_ACK);
						flushinput(serial_read);
						return XMODEM_ERRORS::CANCELED_BY_REMOTE;
					}
					break;
				default:
					break;
				}
			}
		}
#if XMODEM_WRITE_LOG
		fprintf(logFile, "No sync, cancelling\n");
#endif
		(*serial_write)(XMODEM_CAN);
		(*serial_write)(XMODEM_CAN);
		(*serial_write)(XMODEM_CAN);
		flushinput(serial_read);
		return XMODEM_ERRORS::SYNC_ERROR;

		for (;;)
		{
		start_trans:
			// LOG("Preparing packet ");
			// LOGLN((int)packetno);

#if XMODEM_WRITE_LOG
			fprintf(logFile, "Preparing %d\n", packetno);
#endif

			bufsz = 128;

			if (!is_eof)
			{
				for (retry = 0; retry < MAXRETRANS; ++retry)
				{
					LOG("Transmitting packet ");
					LOGLN((int)packetno);

					// fprintf(logFile, "Transmitting %d\n", packetno);

					(*serial_write)(XMODEM_SOH);
					(*serial_write)(packetno);
					(*serial_write)((unsigned char)(~packetno));

					this->packetChecksome = 0;
					this->packetCrc = 0;

					for (i = 0; i < bufsz; ++i)
					{
#if XMODEM_WRITE_LOG
						fprintf(logFile, "Transmitting packet %d byte %d\n", packetno, i);
#endif
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
#if XMODEM_WRITE_LOG
						fprintf(logFile, "Reply to packet transmission %d\n", c);
#endif

						switch (c)
						{
						case XMODEM_ACK:
							LOGLN("RECIEVER SENT ACK");
							++packetno;
							this->packetOffset += bufsz;
							goto start_trans;
						case XMODEM_CAN:
							LOGLN("RECIEVER SENT CAN");
							if ((c = serial_read(DELAY_1000)) == XMODEM_CAN)
							{
								(*serial_write)(XMODEM_ACK);
								flushinput(serial_read);
								return XMODEM_ERRORS::CANCELED_BY_REMOTE;
							}
							break;
						case XMODEM_NAK:
							LOGLN("RECIEVER SENT NAK");
							break;
						default:
							LOGLN("RECIEVER SENT GARBAGE");
							break;
						}
					}
				}
#if XMODEM_WRITE_LOG
				fprintf(logFile, "Transmission error\n");
#endif

				(*serial_write)(XMODEM_CAN);
				(*serial_write)(XMODEM_CAN);
				(*serial_write)(XMODEM_CAN);
				flushinput(serial_read);
				return XMODEM_ERRORS::TRANSMIT_ERROR;
			}
			else
			{
#if XMODEM_WRITE_LOG
				fprintf(logFile, "End of file\n");
#endif
				for (retry = 0; retry < 10; ++retry)
				{
#if XMODEM_WRITE_LOG
					fprintf(logFile, "Transmitting XMODEM_EOT\n");
#endif
					(*serial_write)(XMODEM_EOT);
					if ((c = serial_read(DELAY_2000)) == XMODEM_ACK)
					{
#if XMODEM_WRITE_LOG
						fprintf(logFile, "ACK received\n");
#endif
						break;
					}
				}
				flushinput(serial_read);
				return (c == XMODEM_ACK) ? XMODEM_ERRORS::NONE : XMODEM_ERRORS::NO_EOT_REPLY;
			}
		}
	}
}

#endif
