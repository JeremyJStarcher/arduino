#include "ioserial.h"

void IoSerial::begin(HardwareSerial *_serial) {
  this->serial = _serial;
}

int IoSerial::readbyte() {
  return this->serial->read();
}

void IoSerial::writebyte(uint8_t b) {
  this->serial->write(b);
}
