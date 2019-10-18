#include <Arduino.h>

#include "ioserial.h"

void IoSerial::begin(HardwareSerial *_serial) {
  this->serial = _serial;
}

signed int IoSerial::readbyte(int timeout) {
  long now = millis();
  signed int c;

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
  while (this->serial->read() != -1)
    ;
}
