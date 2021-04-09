#include <LEDMatrixDriver.hpp>

// This sketch will 'flood fill' your LED matrix using the hardware SPI driver Library by Bartosz Bielawski.
// Example written 16.06.2017 by Marko Oette, www.oette.info

// Define the ChipSelect pin for the led matrix (Dont use the SS or MISO pin of your Arduino!)
// Other pins are Arduino specific SPI pins (MOSI=DIN, SCK=CLK of the LEDMatrix) see https://www.arduino.cc/en/Reference/SPI
const uint8_t LEDMATRIX_CS_PIN = 7;

// Number of 8x8 segments you are connecting
const int LEDMATRIX_SEGMENTS = 1;
const int LEDMATRIX_WIDTH    = LEDMATRIX_SEGMENTS * 8;

// The LEDMatrixDriver class instance
LEDMatrixDriver lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN);

void setup() {
  // init the display
  lmd.setEnabled(true);
  lmd.setIntensity(10);   // 0 = low, 10 = high
}




int x = 0, y = 0; // start top left
bool s = true;  // start with led on

void loop() {
  int max_x = 8;
  int max_y = 8;

  lmd.clear();
  lmd.display();

  for (x = 0; x < max_x; x++) {
    setLed(x, 0, true);
    lmd.display();
    delay(100);
  }


}

void setLed(int row, int column, boolean state) {
  /* The board I wired up did not have the signals in
      the right order, so adjust things here.
  */

  row = (row + 1) % 8;
  lmd.setPixel(row, column, state);
}

long bright = 0;
void loop2() {
  // toggle current pixel in framebuffer
  //lmd.setPixel(x, y, s);
  setLed(x, y, s);

  // move to next pixel
  if ( x++ >= LEDMATRIX_WIDTH )
  {
    // Return to left
    x = 0;

    // start new line
    if ( y++ >= 8)
    {
      y = 0;  // need to return to start
      s = !s; // toggle led state
    }
  }


  // Flush framebuffer
  lmd.display();

  lmd.setIntensity((int) floor((bright / 64)) & 0xF); // 0 = low, 10 = high
  bright++;
  delay(10);
}
