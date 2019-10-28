#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>

void _outbyte(int ch);
int _inbyte(long ms);

#include "../xmodem.h"

int comportFD;

long long timeInMilliseconds(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (((long long)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}

void _outbyte(int ch)
{
    char buf[1];
    buf[0] = ch;
    write(comportFD, buf, 1);
}

int _inbyte(long ms)
{
    char buf[1];
    long long now = timeInMilliseconds();
    long long tar = now + ms;
    int rdlen;

    while (1)
    {
        rdlen = read(comportFD, buf, 1);
        if (rdlen > 0)
        {
            return buf[0];
        }

        if (timeInMilliseconds() > tar)
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

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

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
#ifdef MASTER
    const char *portname = MASTER_DEVICE;
    printf("MASTER\n");
#endif

#ifdef SLAVE
    const char *portname = SLAVE_DEVICE;
    printf("SLAVE\n");
#endif

    int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK);
    comportFD = fd;
    if (fd < 0)
    {
        printf("Error opening %s: %s\n", portname, strerror(errno));
        return -1;
    }
    /*baudrate 115200, 8 bits, no parity, 1 stop bit */
    set_interface_attribs(fd, B9600);

    write(fd, "Message from the other side\n", 28);

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
