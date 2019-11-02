#ifndef JDRIVER_H
#define JDRIVER_H
#include <Wire.h>

class Jdriver : public Adafruit_SSD1306 {
    public:
    Jdriver(int w, int h, int8_t RST) : Adafruit_SSD1306(w, h, &Wire, RST)
 { };
 
   
    void print_P(unsigned const char *src);
    void println_P(unsigned const char *src);
    private:
};

#endif
