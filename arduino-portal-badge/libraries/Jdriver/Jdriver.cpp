#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Jdriver.h"


void Jdriver::print_P(unsigned const char *src) {
  char *src2 = (char *) src;
  char khar;
  
  while ((khar = pgm_read_byte(src2)) != 0) {
    Adafruit_SSD1306::write(khar);
    src2++;
  }
}

void Jdriver::println_P(unsigned const char *src) {
  Jdriver::print_P(src);
  Adafruit_SSD1306::println();
}

