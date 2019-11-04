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
   and input/output functions serial_read() and (*serial_write)().

   the prototypes of the input/output functions are:
     int serial_read(unsigned short timeout); // msec timeout
     void (*serial_write)(int c);

*/

#define XMODEM_LOG_NULL 0
#define XMODEM_LOG_IOSTREAM 0
#define XMODEM_LOG_SERIAL0 1

#if XMODEM_LOG_SERIAL0
#include <Arduino.h>

#define LOG(x)              \
	Serial.print("(LOG) "); \
	Serial.print(x)
#define LOGLN(x)     \
	Serial.print(x); \
	Serial.println("");

#endif

#if XMODEM_LOG_NULL
#define LOG(x)
#define LOGLN(x)
#endif

#if XMODEM_LOG_IOSTREAM

#include <iostream>

#define LOG(x) cout << x
#define LOGLN(x) \
	cout << x;   \
	cout << "\n"
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

using namespace std;

#include "xmodem.h"

//enum class XMODEM_SIGNALS : unsigned char
//{
//  SOH = 0x01
//};

#define SOH 0x01
#define STX 0x02
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15
#define CAN 0x18
#define CTRLZ 0x1A

#define DELAY_1000 (1000)
#define DELAY_LONG (1000 * 10)
#define DELAY_1500 (1500)
#define DELAY_2000 (2000)

#define MAXRETRANS 25

#define WRITE_LOG 0

Xmodem::Xmodem(
	int (*_serial_read)(long int ms),
	void (*_serial_write)(int ch))
{
	this->serial_read = _serial_read;
	this->serial_write = _serial_write;
}

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

static void flushinput(int (*serial_read)(long int ms))
{
	unsigned int cnt = 0;
	while (serial_read(DELAY_1500) >= 0)
		cnt++;
#if WRITE_LOG
	fprintf(logFile, "Flushinput (%d flushed)\n", cnt);
#endif
}

int Xmodem::receive(
	unsigned char *dest,
	int destsz)
{
	int bufsz;
	unsigned char trychar = 'C';
	unsigned char packetno = 1;
	int i, c, len = 0;
	int retry, retrans = MAXRETRANS;

	for (;;)
	{
		for (retry = 0; retry < 16; ++retry)
		{
			if (trychar)
			{
#if WRITE_LOG
				fprintf(logFile, "CRC/Checksome start %d\n", trychar);
#endif
				(*serial_write)(trychar);
			}
			if ((c = serial_read(DELAY_2000)) >= 0)
			{
#if WRITE_LOG
				fprintf(logFile, "CRC/Checksome received: %d\n", c);
#endif
				switch (c)
				{
				case SOH:
					LOGLN("RECIEVED SOH (128 byte buffer start)");
					bufsz = 128;
					goto start_recv;
				case STX:
					LOGLN("RECIEVED STX (1024 byte buffer start)");
					bufsz = 1024;
					goto start_recv;
				case EOT:
					LOGLN("RECIEVED EOT");
					flushinput(serial_read);
					(*serial_write)(ACK);
					return len; /* normal end */
				case CAN:
					LOGLN("RECIEVED CAN");
					if ((c = serial_read(DELAY_1000)) == CAN)
					{
						flushinput(serial_read);
						(*serial_write)(ACK);
						return -1; /* canceled by remote */
					}
					break;
				default:
					break;
				}
			}
		}
		if (trychar == 'C')
		{
#if WRITE_LOG
			fprintf(logFile, "Trying to start in checksome mode..\n");
#endif
			trychar = NAK;
			continue;
		}
#if WRITE_LOG
		fprintf(logFile, "Sync error\n");
#endif
		flushinput(serial_read);
		(*serial_write)(CAN);
		(*serial_write)(CAN);
		(*serial_write)(CAN);
		return -2; /* sync error */

	start_recv:
		LOG("Receiving packet ");
		LOG((int)packetno);
		LOG(" retries ");
		LOGLN(retrans);
#if WRITE_LOG
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
#if WRITE_LOG
			fprintf(logFile, "receiving %d byte: %d\n", packetno, i);
#endif
			if ((c = serial_read(DELAY_1000)) < 0)
			{
				LOGLN("REJECT: payload timeout");
#if WRITE_LOG
				fprintf(logFile, "read timeout. rejecting\n");
#endif
				goto reject;
			}
			this->accumulateCrc(c);
			dest[len + i] = c;
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

#if WRITE_LOG
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
#if WRITE_LOG
		fprintf(logFile, "Passed check #1\n");
#endif

		if (incomingPacketNumber == packetno)
		{
			LOGLN("ACCEPTING PACKET");
#if WRITE_LOG
			fprintf(logFile, "Passed check #2\n");
#endif
			register int count = destsz - len;
			if (count > bufsz)
				count = bufsz;
			if (count > 0)
			{
				len += count;
			}
			++packetno;
			retrans = MAXRETRANS + 1;
		}
#if WRITE_LOG
		fprintf(logFile, "Retries left %d\n", retrans);
#endif

		if (--retrans <= 0)
		{
			flushinput(serial_read);
			(*serial_write)(CAN);
			(*serial_write)(CAN);
			(*serial_write)(CAN);
			return -3; /* too many retry error */
		}
#if WRITE_LOG
		fprintf(logFile, "Packet accepted\n");
#endif

		LOGLN("SENDING PACKET ACK");
		(*serial_write)(ACK);
		continue;

	reject:
#if WRITE_LOG
		fprintf(logFile, "Packet rejected\n");
#endif
		LOGLN("SENDING PACKET NAK");
		flushinput(serial_read);
		(*serial_write)(NAK);
	}
}

int Xmodem::transmit(unsigned char *src, size_t srcsz)
{
	int bufsz;
	unsigned char packetno = 1;
	int i, c, len = 0;
	int retry;

	for (;;)
	{
#if WRITE_LOG
		fprintf(logFile, "Start of forever loop\n");
#endif
		for (retry = 0; retry < 16; ++retry)
		{
#if WRITE_LOG
			fprintf(logFile, "Start of retry loop\n");
#endif
			if ((c = serial_read(DELAY_1500)) < 0)
			{
#if WRITE_LOG
				fprintf(logFile, "No reply to packet %d\n", c);
#endif
			}
			else
			{
#if WRITE_LOG
				fprintf(logFile, "Transmit header received %d\n", c);
#endif
				switch (c)
				{
				case 'C':
					LOGLN("RECIEVER WANTS CRC");
					this->useCrc = true;
					goto start_trans;
				case NAK:
					LOGLN("RECIEVER WANTS CHECKSOME");
					this->useCrc = false;
					goto start_trans;
				case CAN:
					LOGLN("RECIEVER CANCELLED");
					if ((c = serial_read(DELAY_1000)) == CAN)
					{
						(*serial_write)(ACK);
						flushinput(serial_read);
						return -1; /* canceled by remote */
					}
					break;
				default:
					break;
				}
			}
		}
#if WRITE_LOG
		fprintf(logFile, "No sync, cancelling\n");
#endif
		(*serial_write)(CAN);
		(*serial_write)(CAN);
		(*serial_write)(CAN);
		flushinput(serial_read);
		return -2; /* no sync */

		for (;;)
		{
		start_trans:
			// LOG("Preparing packet ");
			// LOGLN((int)packetno);

#if WRITE_LOG
			fprintf(logFile, "Preparing %d\n", packetno);
#endif

			bufsz = 128;
			c = srcsz - len;
			if (c > bufsz)
			{
				c = bufsz;
			}

			if (c >= 0)
			{
				for (retry = 0; retry < MAXRETRANS; ++retry)
				{
					LOG("Transmitting packet ");
					LOGLN((int)packetno);

					// fprintf(logFile, "Transmitting %d\n", packetno);

					(*serial_write)(SOH);
					(*serial_write)(packetno);
					(*serial_write)((unsigned char)(~packetno));

					this->packetChecksome = 0;
					this->packetCrc = 0;

					for (i = 0; i < bufsz; ++i)
					{
#if WRITE_LOG
						fprintf(logFile, "Transmitting packet %d byte %d\n", packetno, i);
#endif
						size_t indx = len + i;

						unsigned char ch = (indx < srcsz) ? src[indx] : CTRLZ;

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
#if WRITE_LOG
						fprintf(logFile, "Reply to packet transmission %d\n", c);
#endif

						switch (c)
						{
						case ACK:
							LOGLN("RECIEVER SENT ACK");
							++packetno;
							len += bufsz;
							goto start_trans;
						case CAN:
							LOGLN("RECIEVER SENT CAN");
							if ((c = serial_read(DELAY_1000)) == CAN)
							{
								(*serial_write)(ACK);
								flushinput(serial_read);
								return -1; /* canceled by remote */
							}
							break;
						case NAK:
							LOGLN("RECIEVER SENT NAK");
							break;
						default:
							LOGLN("RECIEVER SENT GARBAGE");
							break;
						}
					}
				}
#if WRITE_LOG
				fprintf(logFile, "Transmission error\n");
#endif

				(*serial_write)(CAN);
				(*serial_write)(CAN);
				(*serial_write)(CAN);
				flushinput(serial_read);
				return -4; /* xmit error */
			}
			else
			{
#if WRITE_LOG
				fprintf(logFile, "End of file\n");
#endif
				for (retry = 0; retry < 10; ++retry)
				{
#if WRITE_LOG
					fprintf(logFile, "Transmitting EOT\n");
#endif
					(*serial_write)(EOT);
					if ((c = serial_read(DELAY_2000)) == ACK)
					{
#if WRITE_LOG
						fprintf(logFile, "ACK received\n");
#endif
						break;
					}
				}
				flushinput(serial_read);
				return (c == ACK) ? len : -5;
			}
		}
	}
}
