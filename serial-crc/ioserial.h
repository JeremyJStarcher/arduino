#ifndef IO_SERIAL_H
#define IO_SERIAL_H

#include "ioline.h"
#include <stdint.h>
#include <HardwareSerial.h>

class IoSerial: public IoLine {
  public:
    int readbyte();
    void writebyte(uint8_t b);
    void begin(HardwareSerial *_serial);

  private:
    HardwareSerial *serial;
};

#endif
