#ifndef IO_LINE_H
#define IO_LINE_H
#include <stdint.h>

class IoLine {
  public:
    virtual signed int readbyte(int timeout) = 0;
    virtual void writebyte(uint8_t b) = 0;
    virtual void flush() = 0;
    void push(int pushValue);
  protected:
    int pushValue;
    bool hasPushValue;

};

#endif
