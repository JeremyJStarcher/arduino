#include "xmodem.h"

#ifndef XMODEM_CRC_FAST
#ifndef XMODEM_CRC_SLOW
#define XMODEM_CRC_SLOW 1
#endif
#endif

const int DELAY_1000 = (1000);
const int DELAY_LONG = (1000 * 10);
const int DELAY_1500 = (1500);
const int DELAY_2000 = (2000);

const unsigned char XMODEM_SOH = 0x01;
const unsigned char XMODEM_STX = 0x02;
const unsigned char XMODEM_EOT = 0x04;
const unsigned char XMODEM_ACK = 0x06;
const unsigned char XMODEM_NAK = 0x15;
const unsigned char XMODEM_CAN = 0x18;
const unsigned char XMODEM_CTRLZ = 0x1A;

const char MAXRETRANS = 25;

unsigned char *xmodemBuffer;
size_t xmodemBufferSize;

int getCharFromFullBuffer(xmodem_t offset, xmodem_t i)
{
	xmodem_t pos = offset + i;
	if (pos < xmodemBufferSize)
	{
		return xmodemBuffer[pos];
	}
	else
	{
		return -1;
	}
}

void putCharInFullBuffer(xmodem_t offset, xmodem_t i, unsigned char ch)
{
	xmodem_t pos = offset + i;
	if (pos < xmodemBufferSize)
	{
		xmodemBuffer[pos] = ch;
	}
}

Xmodem::Xmodem(
	int (*_serial_read)(long int ms),
	void (*_serial_write)(int ch))
{
	this->serial_read = _serial_read;
	this->serial_write = _serial_write;
#ifndef NO_ARDUINO
	this->hasStreamObject = false;
#endif
}

#ifndef NO_ARDUINO
Xmodem::Xmodem(Stream *S)
{
	this->stream = S;
	this->hasStreamObject = true;
}
#endif

void Xmodem::putChar(int ch)
{
#ifndef NO_ARDUINO
	if (!this->hasStreamObject)
	{
		this->serial_write(ch);
		return;
	}

	this->stream->write(ch);
#else
	this->serial_write(ch);
#endif
}

int Xmodem::getChar(long int ms)
{
#ifndef NO_ARDUINO
	if (!this->hasStreamObject)
	{
		return this->serial_read(ms);
	}

	const long long t = millis() + ms;
	int ch;
	while (1)
	{
		ch = this->stream->read();
		if (ch >= 0)
		{
			return ch;
		}

		if (millis() > t)
		{
			return -1;
		}
	}
#else
	return this->serial_read(ms);
#endif
}

void Xmodem::flushinput()
{
	while (this->getChar(DELAY_1500) >= 0)
		; // idle
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
	unsigned short crc = this->packetCrc;
	crc ^= ch << 8;
	for (char i = 0; i < 8; ++i)
	{
		if (crc & 0x8000)
			crc = (crc << 1) ^ 0x1021;
		else
			crc = crc << 1;
	}
	this->packetCrc = crc;
	this->packetChecksome += ch;
}
#endif

void Xmodem::updateStatus(void (*broadcastPacketChange)(XModemPacketStatus status))
{
	XModemPacketStatus status = XModemPacketStatus();
	status.packetNumber = this->fullPacketNumber;
	status.action = this->packetAction;
	(*broadcastPacketChange)(status);
}

XMODEM_TRANSFER_STATUS Xmodem::receiveFullBuffer(
	unsigned char *dest,
	xmodem_t dest_size,
	void (*broadcastPacketChange)(XModemPacketStatus status))
{
	xmodemBuffer = dest;
	xmodemBufferSize = dest_size;

	return receiveCharacterMode(putCharInFullBuffer, broadcastPacketChange);
}

XMODEM_TRANSFER_STATUS Xmodem::receiveCharacterMode(
	void (*storeCharacter)(xmodem_t offset, xmodem_t i, unsigned char ch),
	void (*broadcastPacketChange)(XModemPacketStatus status))
{
	int buffer_size;
	XMODEM_INIT_STATE initState = XMODEM_INIT_STATE::ATTEMPTED_CRC;

	int i, c;
	int retry, retrans = MAXRETRANS;
	this->packetCrc = 0;
	this->packetOffset = 0;
	this->packetno = 1;
	this->fullPacketNumber = 0;

	for (;;)
	{
		this->packetAction = XMODEM_PACKET_ACTION::Sync;
		this->updateStatus(broadcastPacketChange);

		for (retry = 0; retry < 16; ++retry)
		{
			if (initState != XMODEM_INIT_STATE::RESOLVED)
			{
				this->putChar((int)initState);
			}

			if ((c = this->getChar(DELAY_2000)) >= 0)
			{
				switch (c)
				{
				case XMODEM_SOH:
					buffer_size = 128;
					goto start_recv;
				case XMODEM_EOT:
					flushinput();
					this->putChar(XMODEM_ACK);
					return XMODEM_TRANSFER_STATUS::SUCCESS;
				case XMODEM_CAN:
					if ((c = this->getChar(DELAY_1000)) == XMODEM_CAN)
					{
						flushinput();
						this->putChar(XMODEM_ACK);
						return XMODEM_TRANSFER_STATUS::CANCELED_BY_REMOTE;
					}
					break;
				default:
					flushinput();
					break;
				}
			}
		}

		if (initState == XMODEM_INIT_STATE::ATTEMPTED_CRC)
		{
			initState = XMODEM_INIT_STATE::ATTEMPT_CHECKSOME;
			continue;
		}

		flushinput();
		this->putChar(XMODEM_CAN);
		this->putChar(XMODEM_CAN);
		this->putChar(XMODEM_CAN);
		this->packetAction = XMODEM_PACKET_ACTION::SyncError;
		this->updateStatus(broadcastPacketChange);
		return XMODEM_TRANSFER_STATUS::SYNC_ERROR;

	start_recv:
		this->packetAction = XMODEM_PACKET_ACTION::Receiving;
		this->updateStatus(broadcastPacketChange);

		if (initState == XMODEM_INIT_STATE::ATTEMPTED_CRC)
		{
			this->useCrc = true;
		}
		initState = XMODEM_INIT_STATE::RESOLVED;

		unsigned char incomingPacketNumber;
		unsigned char incomingPacketNumber2;
		unsigned char incomingCrcHigh;
		unsigned char incomingCrcLow;

		unsigned char incomingChecksome;

		incomingPacketNumber = this->getChar(DELAY_1000);
		if (c == -1)
		{
			this->packetAction = XMODEM_PACKET_ACTION::Timeout;
			goto reject;
		}

		incomingPacketNumber2 = this->getChar(DELAY_1000);
		if (c == -1)
		{
			this->packetAction = XMODEM_PACKET_ACTION::Timeout;
			goto reject;
		}

		this->packetChecksome = 0;
		this->packetCrc = 0;
		for (i = 0; i < buffer_size; ++i)
		{
			if ((c = this->getChar(DELAY_1000)) < 0)
			{
				this->packetAction = XMODEM_PACKET_ACTION::Timeout;
				goto reject;
			}
			this->accumulateCrc(c);
			(*storeCharacter)(this->packetOffset, i, c);
		}

		if (this->useCrc)
		{
			incomingCrcHigh = this->getChar(DELAY_1000);
			if (c == -1)
			{
				this->packetAction = XMODEM_PACKET_ACTION::Timeout;
				goto reject;
			}

			incomingCrcLow = this->getChar(DELAY_1000);
			if (c == -1)
			{
				this->packetAction = XMODEM_PACKET_ACTION::Timeout;
				goto reject;
			}
		}
		else
		{
			incomingChecksome = this->getChar(DELAY_1000);
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
			this->updateStatus(broadcastPacketChange);
			this->packetOffset += buffer_size;
			++this->packetno;
			++this->fullPacketNumber;
			retrans = MAXRETRANS + 1;
		}
		else
		{
			// A ValidDuplicate happens when the receiver received a packet
			// and sent an ACK, but the ACK got mangled on the way back to the
			// sender, who sends the packet again.
			this->packetAction = XMODEM_PACKET_ACTION::ValidDuplicate;
			this->updateStatus(broadcastPacketChange);
		}

		if (--retrans <= 0)
		{
			flushinput();
			this->putChar(XMODEM_CAN);
			this->putChar(XMODEM_CAN);
			this->putChar(XMODEM_CAN);
			return XMODEM_TRANSFER_STATUS::TOO_MANY_RETRIES;
		}

		this->putChar(XMODEM_ACK);
		continue;

	reject:
		this->updateStatus(broadcastPacketChange);
		flushinput();
		this->putChar(XMODEM_NAK);
	}
}

XMODEM_TRANSFER_STATUS Xmodem::transmitFullBuffer(
	unsigned char *src, xmodem_t source_size,
	void (*broadcastPacketChange)(XModemPacketStatus status))
{
	xmodemBufferSize = source_size;
	xmodemBuffer = src;
	return this->transmitCharacterMode(getCharFromFullBuffer, broadcastPacketChange);
}

XMODEM_TRANSFER_STATUS Xmodem::transmitCharacterMode(
	int (*retrieveCharacter)(xmodem_t offset, xmodem_t i),
	void (*broadcastPacketChange)(XModemPacketStatus status))
{
	int buffer_size;
	int i, c;
	int retry;
	bool is_eof = false;
	this->packetOffset = 0;
	this->packetno = 1;
	this->fullPacketNumber = 0;

	for (;;)
	{
		for (retry = 0; retry < 16; ++retry)
		{
			this->packetAction = XMODEM_PACKET_ACTION::WaitingForReceiver;
			this->updateStatus(broadcastPacketChange);

			if ((c = this->getChar(DELAY_1500)) < 0)
			{
				this->packetAction = XMODEM_PACKET_ACTION::Timeout;
				this->updateStatus(broadcastPacketChange);
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
					if ((c = this->getChar(DELAY_1000)) == XMODEM_CAN)
					{
						this->putChar(XMODEM_ACK);
						flushinput();
						return XMODEM_TRANSFER_STATUS::CANCELED_BY_REMOTE;
					}
					break;
				default:
					break;
				}
			}
		}

		this->putChar(XMODEM_CAN);
		this->putChar(XMODEM_CAN);
		this->putChar(XMODEM_CAN);
		flushinput();
		return XMODEM_TRANSFER_STATUS::SYNC_ERROR;

		for (;;)
		{
		start_trans:
			this->packetAction = XMODEM_PACKET_ACTION::Sync;
			this->updateStatus(broadcastPacketChange);

			buffer_size = 128;

			if (!is_eof)
			{
				for (retry = 0; retry < MAXRETRANS; ++retry)
				{
					this->packetAction = XMODEM_PACKET_ACTION::Transmitting;
					this->updateStatus(broadcastPacketChange);

					this->putChar(XMODEM_SOH);
					this->putChar(packetno);
					this->putChar((unsigned char)(~packetno));

					this->packetChecksome = 0;
					this->packetCrc = 0;

					for (i = 0; i < buffer_size; ++i)
					{
						int khar = (*retrieveCharacter)(this->packetOffset, i);
						if (khar == -1)
						{
							is_eof = true;
						}

						unsigned char ch = (khar == -1) ? (XMODEM_CTRLZ) : (unsigned char)khar;

						this->putChar(ch);
						this->accumulateCrc(ch);
					}

					if (this->useCrc)
					{
						this->putChar((this->packetCrc & 0xFF00) >> 8);
						this->putChar(this->packetCrc & 0xFF);
					}
					else
					{
						this->putChar(this->packetChecksome);
					}

					if ((c = this->getChar(DELAY_LONG)) >= 0)
					{
						switch (c)
						{
						case XMODEM_ACK:
							this->packetAction = XMODEM_PACKET_ACTION::ReceiverACK;
							this->updateStatus(broadcastPacketChange);
							++this->packetno;
							++this->fullPacketNumber;
							this->packetOffset += buffer_size;
							goto start_trans;
						case XMODEM_CAN:
							if ((c = this->getChar(DELAY_1000)) == XMODEM_CAN)
							{
								this->putChar(XMODEM_ACK);
								flushinput();
								return XMODEM_TRANSFER_STATUS::CANCELED_BY_REMOTE;
							}
							break;
						case XMODEM_NAK:
							this->packetAction = XMODEM_PACKET_ACTION::ReceiverNAK;
							this->updateStatus(broadcastPacketChange);
							break;
						default:
							this->packetAction = XMODEM_PACKET_ACTION::ReceiverGarbage;
							this->updateStatus(broadcastPacketChange);
							break;
						}
					}
					else
					{
						this->packetAction = XMODEM_PACKET_ACTION::Timeout;
						this->updateStatus(broadcastPacketChange);
					}
				}

				this->putChar(XMODEM_CAN);
				this->putChar(XMODEM_CAN);
				this->putChar(XMODEM_CAN);
				flushinput();
				return XMODEM_TRANSFER_STATUS::TRANSMIT_ERROR;
			}
			else
			{
				for (retry = 0; retry < 10; ++retry)
				{
					this->putChar(XMODEM_EOT);
					if ((c = this->getChar(DELAY_2000)) == XMODEM_ACK)
					{
						break;
					}
				}
				flushinput();
				return (c == XMODEM_ACK) ? XMODEM_TRANSFER_STATUS::SUCCESS : XMODEM_TRANSFER_STATUS::NO_EOT_REPLY;
			}
		}
	}
}
