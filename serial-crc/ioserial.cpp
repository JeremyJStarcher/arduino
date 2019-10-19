#include <Arduino.h>

#include "ioserial.h"

void IoSerial::begin(HardwareSerial *_serial) {
  this->serial = _serial;
  this->hasPushValue = false;
}

signed int IoSerial::readbyte(int timeout) {
  long now = millis();
  signed int c;

  if (this->hasPushValue) {
    this->hasPushValue = false;
    return this->pushValue;
  }

  while ((c = this->serial->read()) == -1) {
    if (millis() > now + timeout) {
      break;
    }
  }

  return c;
}

void IoSerial::writebyte(uint8_t b) {
  this->serial->write(b);
}

void IoSerial::flush() {
  // If there is still data coming in
  delay(1000);
  this->hasPushValue = false;
  while (this->serial->read() != -1)
    ;
}
