#ifndef IO_SERIAL_H
#define IO_SERIAL_H

#include <stdint.h>
#include <HardwareSerial.h>
#include "ioline.h"

class IoSerial: public IoLine {
  public:
    unsigned int readbyte(int timeout);
    void writebyte(uint8_t b);
    void begin(HardwareSerial *_serial);

  private:
    HardwareSerial *serial;
};

#endif
