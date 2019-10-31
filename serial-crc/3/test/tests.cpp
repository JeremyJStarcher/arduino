#include <stdio.h>
#include <string.h>
#include <array>
#include <iostream>

#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "tests.h"
using namespace std;

long long timeInMilliseconds(void);
void serial_write(int ch);
int serial_read(long ms);
extern FILE *logFile;
extern int comportFD;

#define LOG(x) cout << x
#define LOGLN(x) \
    cout << x;   \
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
    LOG("compareBuffer size: ");
    LOGLN(s);

    for (size_t i = 0; i < s; i++)
    {
        int actual = buffer[i];
        int expected = getBufferByte(i);

        if (actual != expected)
        {
            LOG("idx ");
            LOG(i);
            LOG("\tActual: ");
            LOG(actual);
            LOG("\tExpected: ");
            LOG(expected);
            LOGLN("");

            return false;
        }
    }
    LOGLN("Compare buffer passed.");
    return true;
}

void fillBuffer(unsigned char *buffer, size_t s)
{
    LOG("fillBuffer size: ");
    LOGLN(s);

    for (size_t i = 0; i < s; i++)
    {
        buffer[i] = getBufferByte(i);
        LOG((char)buffer[i]);
    }
}

int send_index;
void tweak_write(int ch)
{
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
                printf("******DELETED BYTE\n");
                break;
            case ALTER_RULE_CHANGE:
                (*serial_write)(r.value);
                handled = 1;
                break;
            case ALTER_RULE_INSERT:
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

class XM_ShouldSucceed : public XMTestBase
{
public:
    static void MasterAction(alter_rule *alter_rules)
    {
        send_index = 0;
        LOGLN("Sending...");
        fillBuffer(buffer, BUFFER_SIZE);
        int ret = xmodemTransmit(buffer, BUFFER_SIZE, tweak_write, serial_read);
        LOG("Transmit result: ");
        LOGLN(ret);
        if (ret < 0)
        {
            isPassing = false;
        }
    };

    static void SlaveAction()
    {
        LOGLN("Receiving...");
        int ret = xmodemReceive(buffer, BUFFER_SIZE, serial_write, serial_read);
        LOG("Receive result: ");
        LOGLN(ret);
        if (ret < 0)
        {
            isPassing = false;
        }

        if (!compareBuffer(buffer, BUFFER_SIZE))
        {
            LOGLN("Buffer compare failed");
            isPassing = false;
        }
    }
};

void testNoGlitches()
{
    printf("--------------------------------\n");
    printf("-           NO GLITCHES        -\n");
    printf("--------------------------------\n");

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
    printf("--------------------------------\n");
    printf("-     testMissingDataByte      -\n");
    printf("--------------------------------\n");

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

void testAll()
{
    isPassing = true;

    testNoGlitches();

    testMissingDataByte();

    if (!isPassing)
    {
        return;
    }
}
