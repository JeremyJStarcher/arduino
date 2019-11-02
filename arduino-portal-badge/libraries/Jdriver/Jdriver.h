#ifndef JDRIVER_H
#define JDRIVER_H

class Jdriver : public Adafruit_SSD1306 {
    public:
    Jdriver(int8_t RST) : Adafruit_SSD1306(RST) { };
    void print_P(unsigned const char *src);
    void println_P(unsigned const char *src);
    private:
};

#endif

