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
	XMODEM_ERRORS receive(unsigned char *dest, xmodem_t destsz);
	XMODEM_ERRORS transmit(unsigned char *src, xmodem_t srcsz);
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

#endif
