#include "ioserial.h"

void IoSerial::begin(HardwareSerial *_serial) {
  this->serial = _serial;
}

int IoSerial::readbyte() {
  if (this->serial->available()) {
    return this->serial->read();
  } else {
    return -1;
  }
}
