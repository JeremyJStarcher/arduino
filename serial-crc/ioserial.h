#ifndef IO_SERIAL_H
#define IO_SERIAL_H

#include "ioline.h"
#include <stdint.h>
#include <HardwareSerial.h>

class IoSerial: public IoLine {
  public:
    int readbyte();
    void begin(HardwareSerial *_serial);

  private:
    HardwareSerial *serial;
};

#endif
