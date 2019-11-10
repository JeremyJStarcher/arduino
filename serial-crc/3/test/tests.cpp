#include <stdio.h>
#include <string.h>
#include <array>
#include <iostream>

#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
using namespace std;

#include "tests.h"

long long timeInMilliseconds(void);
void serial_write(int ch);
int serial_read(long ms);
extern FILE *logFile;
extern int comportFD;

#define F(x) (x)
#define LOG(x) cout << (x)
#define LOGLN(x) \
    cout << (x); \
    cout << "\n"

#include "../xmodem.h"

#define BUFFER_SIZE 2048
static unsigned char buffer[BUFFER_SIZE];

struct alter_rule
{
    int position;
    int action;
    int value;
};

#define ALTER_RULE_MAX 5
#define ALTER_RULE_NONE 0
#define ALTER_RULE_INSERT 1
#define ALTER_RULE_DELETE 2
#define ALTER_RULE_CHANGE 3

alter_rule alter_rules[ALTER_RULE_MAX];

bool isPassing;

void resetAlterRules()
{
    for (int i = 0; i < ALTER_RULE_MAX; i++)
    {
        alter_rules[i].action = ALTER_RULE_NONE;
        alter_rules[i].position = -1;
        alter_rules[i].value = -1;
    }
}

unsigned char getBufferByte(size_t idx)
{
    static unsigned char txt[] = "The quick brown fox";
    return txt[idx % sizeof(txt)];
    // return idx & 0xFF;
}

bool compareBuffer(unsigned char *buffer, size_t s)
{
    LOG(F("compareBuffer size: "));
    LOGLN(s);

    for (size_t i = 0; i < s; i++)
    {
        int actual = buffer[i];
        int expected = getBufferByte(i);

        if (actual != expected)
        {
            LOG(F("idx "));
            LOG(i);
            LOG(F("\tActual: "));
            LOG(actual);
            LOG(F("\tExpected: "));
            LOG(expected);
            LOGLN(F(""));

            return false;
        }
    }
    LOGLN(F("Compare buffer passed."));
    return true;
}

void fillBuffer(unsigned char *buffer, size_t s)
{
    LOG(F("fillBuffer size: "));
    LOGLN(s);

    for (size_t i = 0; i < s; i++)
    {
        buffer[i] = getBufferByte(i);
        // LOG((char)buffer[i]);
    }
}

int send_index;
void tweak_write(int ch)
{
    fprintf(logFile, "send_index = %d\n", send_index);

    char handled = 0;

    for (int i = 0; i < ALTER_RULE_MAX; i++)
    {
        alter_rule r = alter_rules[i];

        if (r.position == send_index)
        {
            switch (r.action)
            {
            case ALTER_RULE_DELETE:
                // Do nothing;
                handled = 1;
                LOG(F("****** DELETED BYTE at "));
                LOGLN(r.position);
                break;
            case ALTER_RULE_CHANGE:
                LOG(F("****** CHANGED BYTE at "));
                LOGLN(r.position);
                (*serial_write)(r.value);
                handled = 1;
                break;
            case ALTER_RULE_INSERT:
                LOG(F("****** INSERTED BYTE at "));
                LOGLN(r.position);
                (*serial_write)(ch);
                (*serial_write)(r.value);
                handled = 1;
                break;
            }
            break;
        }
    }
    if (!handled)
    {
        (*serial_write)(ch);
    }
    send_index++;
}

void update_packet(XModemPacketStatus status)
{
    LOG((int) status.packetNumber);
    LOG(F("\t"));

    switch (status.action)
    {
    case XMODEM_PACKET_ACTION::Receiving:
        LOG(F("Receiving"));
        break;
    case XMODEM_PACKET_ACTION::Timeout:
        LOG(F("Timeout"));
        break;
    case XMODEM_PACKET_ACTION::PacketNumberCorrupt:
        LOG(F("PacketNumberCorrupt"));
        break;
    case XMODEM_PACKET_ACTION::PacketNumberOutOfSequence:
        LOG(F("PacketNumberOutOfSequence"));
        break;
    case XMODEM_PACKET_ACTION::CrcMismatch:
        LOG(F("CrcMismatch"));
        break;
    case XMODEM_PACKET_ACTION::ChecksomeMismatch:
        LOG(F("ChecksomeMismatch"));
        break;
    case XMODEM_PACKET_ACTION::Accepted:
        LOG(F("Accepted"));
        break;
    case XMODEM_PACKET_ACTION::ValidDuplicate:
        LOG(F("ValidDuplicate"));
        break;
    case XMODEM_PACKET_ACTION::ReceiverACK:
        LOG(F("ReceiverACK"));
        break;
    case XMODEM_PACKET_ACTION::ReceiverNAK:
        LOG(F("ReceiverNAK"));
        break;
    case XMODEM_PACKET_ACTION::ReceiverGarbage:
        LOG(F("ReceiverGarbage"));
        break;
    case XMODEM_PACKET_ACTION::Sync:
        LOG(F("Sync"));
        break;
    case XMODEM_PACKET_ACTION::SyncError:
        LOG(F("SyncError"));
        break;
    case XMODEM_PACKET_ACTION::Transmitting:
        LOG(F("Transmitting"));
        break;
    case XMODEM_PACKET_ACTION::WaitingForReceiver:
        LOG(F("WaitingForReceiver"));
        break;
    default:
        LOG(F("Unknown Action"));
        break;
    }
    LOGLN(F(""));
}

class XM_ShouldSucceed : public XMTestBase
{
public:
    static void MasterAction(alter_rule *alter_rules)
    {
        Xmodem xmodem(serial_read, tweak_write);
        send_index = 0;

        LOGLN(F("Sending..."));
        fillBuffer(buffer, BUFFER_SIZE);
        auto ret = xmodem.transmitFullBuffer(buffer, BUFFER_SIZE, update_packet);
        LOG(F("Transmit result: "));
        LOGLN(ret == XMODEM_TRANSFER_STATUS::SUCCESS);
        if (ret != XMODEM_TRANSFER_STATUS::SUCCESS)
        {
            isPassing = false;
        }
    };

    static void SlaveAction()
    {
        Xmodem xmodem(serial_read, serial_write);

        LOGLN(F("Receiving..."));
        auto ret = xmodem.receiveFullBuffer(buffer, BUFFER_SIZE, update_packet);
        LOG(F("Receive result: "));
        LOGLN(ret == XMODEM_TRANSFER_STATUS::SUCCESS);
        if (ret != XMODEM_TRANSFER_STATUS::SUCCESS)
        {
            isPassing = false;
        }

        if (!compareBuffer(buffer, BUFFER_SIZE))
        {
            LOGLN(F("Buffer compare failed"));
            isPassing = false;
        }
    }
};

void testNoGlitches()
{
    LOGLN(F("--------------------------------"));
    LOGLN(F("-           NO GLITCHES        -"));
    LOGLN(F("--------------------------------"));

    resetAlterRules();

#ifdef MASTER
    XM_ShouldSucceed::MasterAction(alter_rules);
#endif

#ifdef SLAVE
    XM_ShouldSucceed::SlaveAction();
#endif
}

void testMissingDataByte()
{
    LOGLN(F("--------------------------------"));
    LOGLN(F("-     testMissingDataByte      -"));
    LOGLN(F("--------------------------------"));

    resetAlterRules();
    alter_rules[0].action = ALTER_RULE_DELETE;
    alter_rules[0].position = 17;

#ifdef MASTER
    XM_ShouldSucceed::MasterAction(alter_rules);
#endif

#ifdef SLAVE
    XM_ShouldSucceed::SlaveAction();
#endif
}

void testExtraDataByte()
{
    LOGLN(F("--------------------------------"));
    LOGLN(F("-      testExtraDataByte       -"));
    LOGLN(F("--------------------------------"));

    resetAlterRules();
    alter_rules[0].action = ALTER_RULE_INSERT;
    alter_rules[0].position = 17;
    alter_rules[0].value = 0xFF;

#ifdef MASTER
    XM_ShouldSucceed::MasterAction(alter_rules);
#endif

#ifdef SLAVE
    XM_ShouldSucceed::SlaveAction();
#endif
}

void testChangedByteInPayload()
{
    LOGLN(F("--------------------------------"));
    LOGLN(F("-   testChangedByteInPayload   -"));
    LOGLN(F("--------------------------------"));

    resetAlterRules();
    alter_rules[0].action = ALTER_RULE_CHANGE;
    alter_rules[0].position = 17;
    alter_rules[0].value = 0xFF;

#ifdef MASTER
    XM_ShouldSucceed::MasterAction(alter_rules);
#endif

#ifdef SLAVE
    XM_ShouldSucceed::SlaveAction();
#endif
}

void testAll()
{
    isPassing = true;

    if (isPassing)
        testNoGlitches();

    if (isPassing)
        testMissingDataByte();

    if (isPassing)
        testExtraDataByte();

    if (isPassing)
        testChangedByteInPayload();

    if (isPassing)
    {
        LOGLN(F("--------------------------------"));
        LOGLN(F("-********** Success ********** -"));
        LOGLN(F("--------------------------------"));
    }
    else
    {
        LOGLN(F("--------------------------------"));
        LOGLN(F("- ======= Tests Failed ======= -"));
        LOGLN(F("--------------------------------"));
    }
}
