#ifndef IO_LINE_H
#define IO_LINE_H
#include <stdint.h>

class IoLine {
  public:
    virtual int readbyte() = 0;
    virtual void writebyte(uint8_t b) = 0;
};

#endif
