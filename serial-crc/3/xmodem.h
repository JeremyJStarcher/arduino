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

#define SOH 0x01
#define STX 0x02
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15
#define CAN 0x18
#define CTRLZ 0x1A

#define DLY_1S (1000)
#define DELAY_LONG (1000 * 10)

#define MAXRETRANS 25

unsigned short crc16_ccitt(const unsigned char *buf, int len)
{
	unsigned short crc = 0;
	while (len--)
	{
		int i;
		crc ^= *(char *)buf++ << 8;
		for (i = 0; i < 8; ++i)
		{
			if (crc & 0x8000)
				crc = (crc << 1) ^ 0x1021;
			else
				crc = crc << 1;
		}
	}
	return crc;
}

static int check(int crc, const unsigned char *buf, int sz)
{
	if (crc)
	{
		unsigned short crc = crc16_ccitt(buf, sz);
		unsigned short tcrc = (buf[sz] << 8) + buf[sz + 1];
		if (crc == tcrc)
		{
			fprintf(logFile, "CRC Check... passed\n");
			return 1;
		}
	}
	else
	{
		int i;
		unsigned char cks = 0;
		for (i = 0; i < sz; ++i)
		{
			cks += buf[i];
		}
		if (cks == buf[sz])
		{
			fprintf(logFile, "Checksome passed\n");
			return 1;
		}
	}
	fprintf(logFile, "CRC/Checksome failed\n");
	return 0;
}

static void flushinput(int (*serial_read)(long int ms))
{
	unsigned int cnt = 0;
	while (serial_read(((DLY_1S)*3) >> 1) >= 0)
		cnt++;

	fprintf(logFile, "Flushinput (%d flushed)\n", cnt);
}

int xmodemReceive(
	unsigned char *dest,
	int destsz,
	void (*serial_write)(int ch),
	int (*serial_read)(long int ms))
{
	unsigned char xbuff[1030]; /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */
	unsigned char *p;
	int bufsz, crc = 0;
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
				fprintf(logFile, "CRC/Checksome start %d\n", trychar);
				(*serial_write)(trychar);
			}
			if ((c = serial_read((DLY_1S) << 1)) >= 0)
			{
				fprintf(logFile, "CRC/Checksome received: %d\n", c);
				switch (c)
				{
				case SOH:
					bufsz = 128;
					goto start_recv;
				case STX:
					bufsz = 1024;
					goto start_recv;
				case EOT:
					flushinput(serial_read);
					(*serial_write)(ACK);
					return len; /* normal end */
				case CAN:
					if ((c = serial_read(DLY_1S)) == CAN)
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
			fprintf(logFile, "Trying to start in checksome mode..\n");
			trychar = NAK;
			continue;
		}
		fprintf(logFile, "Sync error\n");
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

		fprintf(logFile, "Start receive packet: %d\n", packetno);
		if (trychar == 'C')
		{
			crc = 1;
		}
		trychar = 0;
		p = xbuff;
		*p++ = c;

		for (i = 0; i < (bufsz + (crc ? 1 : 0) + 3); ++i)
		{
			fprintf(logFile, "receiving %d byte: %d\n", packetno, i);
			if ((c = serial_read(DLY_1S)) < 0)
			{
				fprintf(logFile, "read timeout. rejecting\n");
				goto reject;
			}
			*p++ = c;
		}
		fprintf(logFile, "Entire packet read\n");

		if (xbuff[1] == (unsigned char)(~xbuff[2]) &&
			(xbuff[1] == packetno || xbuff[1] == (unsigned char)packetno - 1) &&
			check(crc, &xbuff[3], bufsz))
		{
			fprintf(logFile, "Passed check #1\n");

			if (xbuff[1] == packetno)
			{
				fprintf(logFile, "Passed check #2\n");
				register int count = destsz - len;
				if (count > bufsz)
					count = bufsz;
				if (count > 0)
				{
					memcpy(&dest[len], &xbuff[3], count);
					len += count;
				}
				++packetno;
				retrans = MAXRETRANS + 1;
			}
			fprintf(logFile, "Retries left %d\n", retrans);
			if (--retrans <= 0)
			{
				flushinput(serial_read);
				(*serial_write)(CAN);
				(*serial_write)(CAN);
				(*serial_write)(CAN);
				return -3; /* too many retry error */
			}
			fprintf(logFile, "Packet accepted\n");
			(*serial_write)(ACK);
			continue;
		}
	reject:
		fprintf(logFile, "Packet rejected\n");
		flushinput(serial_read);
		(*serial_write)(NAK);
	}
}

int xmodemTransmit(unsigned char *src,
				   int srcsz,
				   void (*serial_write)(int ch),
				   int (*serial_read)(long int ms))
{
	unsigned char xbuff[1030]; /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */
	int bufsz, crc = -1;
	unsigned char packetno = 1;
	int i, c, len = 0;
	int retry;

	for (;;)
	{
		fprintf(logFile, "Start of forever loop\n");
		for (retry = 0; retry < 16; ++retry)
		{
			fprintf(logFile, "Start of retry loop\n");
			if ((c = serial_read((DLY_1S) << 1)) < 0)
			{
				fprintf(logFile, "No reply to packet %d\n", c);
			}
			else
			{
				fprintf(logFile, "Transmit header received %d\n", c);

				switch (c)
				{
				case 'C':
					crc = 1;
					goto start_trans;
				case NAK:
					crc = 0;
					goto start_trans;
				case CAN:
					if ((c = serial_read(DLY_1S)) == CAN)
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
		fprintf(logFile, "No sync, cancelling\n");
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

			fprintf(logFile, "Preparing %d\n", packetno);

			xbuff[0] = SOH;
			bufsz = 128;
			xbuff[1] = packetno;
			xbuff[2] = ~packetno;
			c = srcsz - len;
			if (c > bufsz)
				c = bufsz;
			if (c >= 0)
			{
				memset(&xbuff[3], 0, bufsz);
				if (c == 0)
				{
					xbuff[3] = CTRLZ;
				}
				else
				{
					memcpy(&xbuff[3], &src[len], c);
					if (c < bufsz)
						xbuff[3 + c] = CTRLZ;
				}
				if (crc)
				{
					unsigned short ccrc = crc16_ccitt(&xbuff[3], bufsz);
					xbuff[bufsz + 3] = (ccrc >> 8) & 0xFF;
					xbuff[bufsz + 4] = ccrc & 0xFF;
				}
				else
				{
					unsigned char ccks = 0;
					for (i = 3; i < bufsz + 3; ++i)
					{
						ccks += xbuff[i];
					}
					xbuff[bufsz + 3] = ccks;
				}

				for (retry = 0; retry < MAXRETRANS; ++retry)
				{
					LOG("Transmitting packet ");
					LOGLN((int)packetno);

					// fprintf(logFile, "Transmitting %d\n", packetno);

					for (i = 0; i < bufsz + 4 + (crc ? 1 : 0); ++i)
					{
						fprintf(logFile, "Transmitting packet %d byte %d\n", packetno, i);
						(*serial_write)(xbuff[i]);
					}

					if ((c = serial_read(DELAY_LONG)) >= 0)
					{
						fprintf(logFile, "Reply to packet transmission %d\n", c);

						switch (c)
						{
						case ACK:
							++packetno;
							len += bufsz;
							goto start_trans;
						case CAN:
							if ((c = serial_read(DLY_1S)) == CAN)
							{
								(*serial_write)(ACK);
								flushinput(serial_read);
								return -1; /* canceled by remote */
							}
							break;
						case NAK:
						default:
							break;
						}
					}
				}
				fprintf(logFile, "Transmission error\n");

				(*serial_write)(CAN);
				(*serial_write)(CAN);
				(*serial_write)(CAN);
				flushinput(serial_read);
				return -4; /* xmit error */
			}
			else
			{
				fprintf(logFile, "End of file\n");
				for (retry = 0; retry < 10; ++retry)
				{
					fprintf(logFile, "Transmitting EOT\n");
					(*serial_write)(EOT);
					if ((c = serial_read((DLY_1S) << 1)) == ACK)
					{
						fprintf(logFile, "ACK received\n");
						break;
					}
				}
				flushinput(serial_read);
				return (c == ACK) ? len : -5;
			}
		}
	}
}
