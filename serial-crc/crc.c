#include <Arduino.h>

// Compute the MODBUS RTU CRC
uint16_t ModRTU_CRC(uint8_t *buf, size_t len)
{
  uint16_t crc = 0xFFFF;

  for (size_t pos = 0; pos < len; pos++) {
    crc ^= (uint16_t)buf[pos];          // XOR byte into least sig. byte of crc

    for (int i = 8; i != 0; i--) {    // Loop over each bit
      if ((crc & 0x0001) != 0) {      // If the LSB is set
        crc >>= 1;                    // Shift right and XOR 0xA001
        crc ^= 0xA001;
      }
      else                            // Else LSB is not set
        crc >>= 1;                    // Just shift right
    }
  }
  // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
  return crc;
}

uint8_t getMSB(uint16_t x) {
  return (uint8_t)(x >> 8);
}

uint8_t getLSB(uint16_t x) {
  return x & 0xff;
}
