
//One 24LC256 EEPROMs on the bus
const uint32_t totalKBytes = 32;         //for read and write test functions
extEEPROM eep(kbits_256, 1, 64);         //device size, number of devices, page size

// Here we define the four pieces of information that OSFS needs to make a filesystem:
//
// 1) and 2) How large is the storage medium?
uint16_t OSFS::startOfEEPROM = 1;
uint16_t OSFS::endOfEEPROM = totalKBytes * 1024;

// 3) How do I read from the medium?
void OSFS::readNBytes(uint16_t address, unsigned int num, byte* output) {
  eep.read(address, output, num);
}

// 4) How to I write to the medium?
void OSFS::writeNBytes(uint16_t address, unsigned int num, const byte* input) {
  eep.write(address, input, num);
}

// For the breakout board, you can use any 2 or 3 pins.
// These pins will also work for the 1.8" TFT shield.
#define TFT_CS 10
#define TFT_RST 9 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC 14


#define TFT_MOSI 16
#define TFT_SCLK 15

// For ST7735-based displays, we will use this call
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

const byte KEYBOARD_ROWS = 4; // KEYBOARD_ROWS
const byte KEYBOARD_COLS = 4; // columns
//define the symbols on the buttons of the keypads
char keys[KEYBOARD_ROWS][KEYBOARD_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte colPins[KEYBOARD_ROWS] = {4, 5, 6, 7}; //connect to the row pinouts of the keypad
byte rowPins[KEYBOARD_COLS] = {8, A3, A2, A1}; //connect to the column pinouts of the keypad
