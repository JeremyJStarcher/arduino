#include <stdio.h>
#include <string.h>
#include <array>
#include <iostream>

#include "tests.h"
using namespace std;

void _outbyte(int ch);
int _inbyte(long ms);
extern FILE *logFile;

#define LOG(x) cout << x
#define LOGLN(x) \
    cout << x;   \
    cout << "\n"

#include "../xmodem.h"

#define BUFFER_SIZE 2048 // 128*2
static unsigned char buffer[BUFFER_SIZE];

bool isPassing;
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
        LOG((char) buffer[i]);
    }
}

void testAll()
{
    isPassing = true;

    if (!isPassing)
    {
        return;
    }

#ifdef MASTER
    LOGLN("Sending...");
    fillBuffer(buffer, BUFFER_SIZE);
    int ret = xmodemTransmit(buffer, BUFFER_SIZE);
    LOG("Transmit result: ");
    LOGLN(ret);
    if (ret < 0)
    {
        isPassing = false;
    }
#endif

#ifdef SLAVE
    LOGLN("Receiving...");
    int ret = xmodemReceive(buffer, BUFFER_SIZE);
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
#endif
}
