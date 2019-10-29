#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>

#include "tests.h"

unsigned char handshakeMessage[] = "Message from the other side.\n|||\n";

FILE *logFile;

int comportFD;
long sendCount = 0;
long receiveCount = 0;

long long timeInMilliseconds(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (((long long)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}

const int glitchbyte = 128 + 10;

void _outbyte(int ch)
{
    char buf[1];
    buf[0] = ch;
    sendCount++;

    if (sendCount == glitchbyte)
    {
        printf("\nMANGLE PACKET\n");
        buf[0] = ch + 2;
        fprintf(logFile, "w: %u\n", ch);
        write(comportFD, buf, 1);
        fprintf(logFile, "w: %u\n", ch);
        write(comportFD, buf, 1);
    }
    else
    {
        fprintf(logFile, "w: %u\n", ch);
        write(comportFD, buf, 1);
    }

    tcdrain(comportFD);
}

int _inbyte(long ms)
{
    char buf[10];
    long long target = timeInMilliseconds() + ms;
    int rdlen;

    while (1)
    {
        rdlen = read(comportFD, buf, 1);
        if (rdlen > 0)
        {
            receiveCount++;
            unsigned char k = buf[0];
            //if (receiveCount > glitchbyte - 5 and receiveCount < glitchbyte + 5)
            //{
            //    printf("===RECEIVED %d %u\n", receiveCount, k);
            //}

            fprintf(logFile, "r: %u\n", k);
            return k;
        }

        if (timeInMilliseconds() > target)
        {
            return -1;
        }
    }
}

int set_interface_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0)
    {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD); /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;      /* 8-bit characters */
    tty.c_cflag &= ~PARENB;  /* no parity bit */
    tty.c_cflag &= ~CSTOPB;  /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS; /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;                                                        // Disable echo
    tty.c_lflag &= ~ECHOE;                                                       // Disable erasure
    tty.c_lflag &= ~ECHONL;                                                      // Disable new-line echo
    tty.c_lflag &= ~ISIG;                                                        // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);                                      // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
                           // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT IN LINUX)
                           // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT IN LINUX)

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

void set_mincount(int fd, int mcount)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0)
    {
        printf("Error tcgetattr: %s\n", strerror(errno));
        return;
    }

    tty.c_cc[VMIN] = mcount ? 1 : 0;
    tty.c_cc[VTIME] = 5; /* half second timer */

    if (tcsetattr(fd, TCSANOW, &tty) < 0)
        printf("Error tcsetattr: %s\n", strerror(errno));
}

int main()
{
    char cmdBuf[80];
#ifdef MASTER
    printf("MASTER\n");
    logFile = fopen("master.txt", "w");
    const char *portname = MASTER_DEVICE;
#endif

#ifdef SLAVE
    printf("SLAVE\n");
    logFile = fopen("slave.txt", "w");
    const char *portname = SLAVE_DEVICE;
#endif

#ifndef MASTER
#ifndef SLAVE
    const char *portname = "NONE";
#endif
#endif

    int counter = 0;

    int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK);
    comportFD = fd;
    if (fd < 0)
    {
        printf("Error opening %s: %s\n", portname, strerror(errno));
        return -1;
    }
    /*baudrate 115200, 8 bits, no parity, 1 stop bit */
    set_interface_attribs(fd, B9600);

#ifdef MASTER
    //sprintf(cmdBuf, "stty -F%s 9600 raw -echo", MASTER_DEVICE);
    //system(cmdBuf);
#endif

#ifdef SLAVE
    // sprintf(cmdBuf, "stty -F%s 9600 raw -echo", SLAVE_DEVICE);
    // system(cmdBuf);
#endif

    printf("Configur comm port: \n%s\n", cmdBuf);

    write(fd, handshakeMessage, sizeof handshakeMessage);

    tcdrain(fd); /* delay for output */

    /* simple noncanonical input */
    do
    {
        unsigned char buf[2];
        int rdlen;

        int jj = _inbyte(1000);
        if (jj == -1)
        {
            rdlen = 0;
        }
        else
        {
            rdlen = 1;
            buf[0] = jj;
            if (jj == '|')
            {
                counter++;
                if (counter == 3)
                {
                    printf("FLAG FOUND. READY TO START TESTS\n");
                    receiveCount = 0;
                    sendCount = 0;

                    while (_inbyte(1000) != -1)
                    {
                        // Idle
                    }

                    testAll();

                    printf("Busy loop\n");
                    while (1)
                    {
                        ; // Do nothing
                    }
                }
            }
        }

        if (rdlen > 0)
        {
            buf[rdlen] = 0;
            printf("%s", buf);
        }
        else if (rdlen < 0)
        {
            //    printf("Error from read: %d: %s\n", rdlen, strerror(errno));
        }
        else
        { /* rdlen == 0 */
            //  printf("Timeout from read\n");
        }
        /* repeat read to get full message */
    } while (1);
}
