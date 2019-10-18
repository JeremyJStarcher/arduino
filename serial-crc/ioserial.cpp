#include <Arduino.h>

#include "ioserial.h"

void IoSerial::begin(HardwareSerial *_serial) {
  this->serial = _serial;
}

unsigned int IoSerial::readbyte(int timeout) {
  long now = millis();
  unsigned int c;

  while ((c = this->serial->read()) == -1) {
    if (millis() > now + timeout) {
      return -1;
    }
  }

  return c;
}

void IoSerial::writebyte(uint8_t b) {
  this->serial->write(b);
}
