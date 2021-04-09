#include <LEDMatrixDriver.hpp>

// This sketch will 'flood fill' your LED matrix using the hardware SPI driver Library by Bartosz Bielawski.
// Example written 16.06.2017 by Marko Oette, www.oette.info

// Define the ChipSelect pin for the led matrix (Dont use the SS or MISO pin of your Arduino!)
// Other pins are Arduino specific SPI pins (MOSI=DIN, SCK=CLK of the LEDMatrix) see https://www.arduino.cc/en/Reference/SPI
// nano 11 - MOSI -> DIN
// nano 13 - SCK -> SCK
const uint8_t LEDMATRIX_CS_PIN = 7;

// Number of 8x8 segments you are connecting
const int LEDMATRIX_SEGMENTS = 2;
const int LEDMATRIX_WIDTH    = LEDMATRIX_SEGMENTS * 8;
const int LEDMATRIX_HEIGHT   = 8;

// The LEDMatrixDriver class instance
LEDMatrixDriver lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN);


const int BRIGHT = 0x00; // 0x0F - 0x0F
const int DELAY = 150;
const int MAX_ITTER = 150;

void setup() {
  randomSeed(analogRead(0));
  lmd.setEnabled(true);
  lmd.setIntensity(BRIGHT);
}

void loop() {
  game(LEDMATRIX_WIDTH, LEDMATRIX_HEIGHT); //w,h
}

void setDot(int x, int y, bool isOn) {
  setLed(x, y, isOn);
}

void setLed(int row, int column, boolean state) {
  /* The board I wired up did not have the signals in
      the right order, so adjust things here.
  */

  row = (row + 1);
  if (row == 8) row -= 8;
  if (row == 16) row -= 8;
  lmd.setPixel(row, column, state);
}


bool show(void *u, int w, int h) {
  bool worldEmpty = true;
  int (*univ)[w] = u;
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      bool sh = (univ[y][x] == 1);
      setDot(x, y, sh);
      if (sh) {
        worldEmpty = false;
      }
    }
  }
  lmd.display();
  return worldEmpty;
}

void evolve(void *u, int w, int h) {
  unsigned (*univ)[w] = u;
  unsigned newar[h][w];

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      int n = 0;
      for (int y1 = y - 1; y1 <= y + 1; y1++)
        for (int x1 = x - 1; x1 <= x + 1; x1++)
          if (univ[(y1 + h) % h][(x1 + w) % w])
            n++;

      if (univ[y][x]) n--;
      newar[y][x] = (n == 3 || (n == 2 && univ[y][x]));
    }
  }

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      univ[y][x] = newar[y][x];
    }
  }
}

bool flash(void *u, int w, int h, bool mode) {
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      setDot(x, y, mode);
    }
  }
  lmd.display();
}

void game(int w, int h) {
  unsigned univ[h][w];
  for (int x = 0; x < w; x++) {
    for (int y = 0; y < h; y++) {
      univ[y][x] = random(0, 100) > 65 ? 1 : 0;
    }
  }
  int sc = 0;
  while (1) {
    bool worldEmpty = show(univ, w, h);
    if (worldEmpty) {
      break;
    }
    evolve(univ, w, h);
    delay(DELAY);
    sc++;
    if (sc > MAX_ITTER) {
      break;
    }
  }
  lmd.setIntensity(BRIGHT);
  flash(univ, w, h, true);
  delay(DELAY);
  lmd.setIntensity(BRIGHT);
}
