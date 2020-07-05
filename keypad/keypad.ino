/**************************************************************************
  This is a library for several Adafruit displays based on ST77* drivers.

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <EEPROM.h>

#include <Keypad.h>
#include <Keyboard.h>

// For the breakout board, you can use any 2 or 3 pins.
// These pins will also work for the 1.8" TFT shield.
#define TFT_CS        10
#define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         8
#define TFT_SDA       16 // **HARDWARE PIN MOSI
#define TFT_SDL       15 // **HARDWARE PIN SCK
// VCC
// GND

// RESET BUTTON
// RST
// GND

// FTDI
// DTR    N/C
// RX     TX1
// TX     RX1
// VCC    VCC
// CTX    GND
// GND    GND


Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

const byte KEY_ROWS = 4;
const byte KEY_COLS = 3;
char keys[KEY_ROWS][KEY_COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

/*
   Starting at # character and working the way down
   A3
   A2
   A1
   7
   6
   5
   4
*/

byte rowPins[KEY_ROWS] = {4, 5, 6, 7};
byte colPins[KEY_COLS] = { A1, A2, A3 };

// How many total memory chucks to device the EEPROM into
const byte EEPROM_SLOTS = 16;
const byte PASSWORD_SLOT = 10;

const int slot_size = EEPROM.length() / EEPROM_SLOTS;
const int display_name_length = 10;
const int secret_length = slot_size - display_name_length;

Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, KEY_ROWS, KEY_COLS);

float p = 3.1415926;

bool isLocked = true;

void draw_screen(void) {
  if (isLocked) {
    ttf_locked_screen();
  } else {
    ttf_names();
  }
}


void ttf_locked_screen(void) {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextWrap(false);
  byte fontSize = 2;
  tft.setTextSize(fontSize);

  tft.setTextColor(ST77XX_BLACK, ST77XX_YELLOW);
  tft.println("  SCREEN  ");
  tft.println("  LOCKED  ");
  tft.println("");

  tft.setTextColor(ST77XX_RED);
  tft.print("  *");
  tft.setTextColor(ST77XX_GREEN);
  tft.print("<PIN>");
  tft.setTextColor(ST77XX_RED);
  tft.println("# ");
}

void ttf_names(void) {
  tft.fillScreen(ST77XX_YELLOW);
  tft.setCursor(0, 0);
  tft.setTextWrap(false);
  byte fontSize = 2;
  tft.setTextSize(fontSize);

  for (int i = 0; i < 10; i++) {
    int text_color = i % 2 == 0 ? ST77XX_WHITE : ST77XX_WHITE;
    int bg_color = i % 2 == 0 ? ST77XX_BLUE : ST77XX_BLACK;

    tft.fillRect(0, i * 8 * fontSize , tft.width(), (8 * fontSize) - 1, bg_color);

    tft.setTextColor(text_color);
    tft.print("#");
    tft.print(i);
    tft.print(" ");

    int offset = get_name_offset(i);

    char c;
    while ((c = EEPROM.read(offset )) != 0) {
      tft.print(c);
      offset += 1;
    }

    tft.println("");
  }
}

void setup(void) {
  Serial.begin(9600);

  Serial1.begin(9600);

  Serial1.println(F("SYSTEM LOADED"));

  Keyboard.begin();

  // Serial.print(F("Hello! ST77xx TFT Test"));

  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab

  draw_screen();

  // SPI speed defaults to SPI_DEFAULT_FREQ defined in the library, you can override it here
  // Note that speed allowable depends on chip and quality of wiring, if you go too fast, you
  // may end up with a black screen some times, or all the time.
  //tft.setSPISpeed(40000000);

  return;
}

void loop2() {
  tft.invertDisplay(true);
  delay(500);
  tft.invertDisplay(false);
  delay(500);
}

int get_name_offset(int slot) {
  return slot_size * slot;
}

int get_secret_offset(int slot) {
  return get_name_offset(slot) + display_name_length;
}

void update_eprom(int offset, char *str) {
  for (int i = 0; i < strlen(str); i++) {
    EEPROM.update(offset + i, str[i]);
    EEPROM.update(offset + i + 1, 0);
  }
}

void save_name(int slot, char *str) {
  update_eprom(get_name_offset(slot), str);
}

void save_secret(int slot, char *str) {
  update_eprom(get_secret_offset(slot), str);
}


void handle_command(char *cmd_str) {
  const byte MAX_TOKENS = 10;

  char *strings[MAX_TOKENS];
  char *ptr = NULL;
  int index = 0;

  ptr = strtok(cmd_str, " ");
  while (ptr != NULL && index < MAX_TOKENS) {
    strings[index] = ptr;
    index += 1;
    ptr = strtok(NULL, " ");
  }

  char *cmd = strings[0];

  if (0) {
    for (int i = 0; i < index; i++) {
      Serial1.print(F("Token #"));
      Serial1.print(i);
      Serial1.print(F(" "));
      Serial1.println(strings[i]);
    }
  }

  if (strcmp("help", cmd) == 0) {
    Serial1.println(F("Valid commands: "));
    Serial1.println(F("  info                  -- system information."));
    Serial1.println(F("  init                  -- clears the entire system."));
    Serial1.println(F("  setname <#> <value>   -- Set display name"));
    Serial1.println(F("  setsecret <#> <value> -- Set value to type"));
    Serial1.println(F("  setpassword <value>   -- Set password"));
    Serial1.println(F("  list                  -- List information"));

    return;
  }

  if (strcmp("list", cmd) == 0) {
    for (int i = 0; i < 10; i++) {
      Serial1.print("#");
      Serial1.print(i);
      Serial1.print(" ");

      int offset = get_name_offset(i);
      int idx = 0;
      char c;
      while ((c = EEPROM.read(offset + idx)) != 0) {
        Serial1.print(c);
        idx += 1;
      }
      while (idx < display_name_length) {
        Serial1.print(" ");
        idx += 1;
      }

      Serial1.print(": ");

      offset = get_secret_offset(i);
      idx = 0;
      while ((c = EEPROM.read(offset + idx)) != 0) {
        Serial1.print(c);
        idx += 1;
      }

      Serial1.println("");
    }
  }

  if (strcmp("setpassword", cmd) == 0) {
    int slot = PASSWORD_SLOT;

    char *value = strings[1];
    if (strlen(value) > display_name_length + display_name_length) {
      Serial1.println(F("The password is too long"));
      return;
    }

    save_name(slot, value);
    return;
  }

  if (strcmp("setname", cmd) == 0) {
    int slot = strings[1][0] - '0';
    if (slot > 9 or slot < 0) {
      Serial1.println(F("Invalid slot number."));
      return;
    }
    char *value = strings[2];
    if (strlen(value) > display_name_length) {
      Serial1.println(F("The label is too long."));
      return;
    }

    save_name(slot, value);
    return;
  }

  if (strcmp("setsecret", cmd) == 0) {
    int slot = strings[1][0] - '0';
    if (slot > 9 or slot < 0) {
      Serial1.println(F("Invalid slot number."));
      return;
    }
    char *value = strings[2];
    if (strlen(value) > secret_length) {
      Serial1.println(F("The secret is too long."));
      return;
    }

    save_secret(slot, value);
    return;
  }

  if (strcmp("info", cmd) == 0) {
    Serial1.print(F("EEPROM size: "));
    Serial1.println(EEPROM.length());

    Serial1.print(F("Slots      : "));
    Serial1.println(EEPROM_SLOTS);

    Serial1.print(F("Slot size  : "));
    Serial1.println(slot_size);

    Serial1.print(F("Name Len   : "));
    Serial1.println(display_name_length);

    Serial1.print(F("Secret Len : "));
    Serial1.println(secret_length);
    Serial1.println("");
    return;
  }

  if (strcmp("init", cmd) == 0) {
    Serial1.println(F("Clearing system:"));
    for (int i = 0 ; i < EEPROM.length() ; i++) {
      EEPROM.update(i, 0);
    }
    Serial1.println(F("EEPROM CLEARED"));
    return;
  }
}

void handle_serial() {
  static const byte CMD_LEN = 100;
  static char serial_cmd[CMD_LEN + 1];

  static byte cmd_index = 0;
  char in_char;

  while (Serial1.available() > 0) {
    in_char = Serial1.read();

    if (1) {
      if (in_char == 8 && cmd_index > 0) {
        Serial1.print(in_char);
        Serial1.print(F(" "));
        Serial1.print(in_char);
        cmd_index -= 1;
        serial_cmd[cmd_index] = 0;
      } else if (in_char == 13) {
        Serial1.println(F(""));
        Serial1.print(F("Your command: '"));
        Serial1.print(serial_cmd);
        Serial1.println(F("'"));
        Serial1.println(F(""));

        handle_command(serial_cmd);

        cmd_index = 0;
        serial_cmd[cmd_index] = 0;
      } else if (in_char == 10) {
        // Ignore line feeds
      } else if (cmd_index < CMD_LEN) {
        serial_cmd[cmd_index] = in_char;
        cmd_index += 1;
        serial_cmd[cmd_index] = 0;
        Serial1.print(in_char);
      }
    }
  }
}

int read_keypad() {
  char key = kpd.getKey();
  if (key) {
    tft.invertDisplay(true);
    delay(100);
    tft.invertDisplay(false);
  }
  return key;
}

void flicker_screen() {
  for (int i = 0; i < 100; i++) {
    tft.invertDisplay(true);
    delay(10);
    tft.invertDisplay(false);
    delay(10);
  }
}

void handle_locked_keypad() {
  static byte idx = 0;
  static bool isValid = false;

  int offset = get_name_offset(PASSWORD_SLOT);

  char key = read_keypad();
  if (!key) {
    return;
  }
  switch (key) {
    case '*':
      idx = 0;
      isValid = true;
      break;
    case '#':
      // '#' needs to match to a zero to show the end the string
      if (EEPROM.read(offset + idx) != 0) {
        isValid = false;
      }

      if (isValid) {
        isLocked = false;
        draw_screen();
      } else {
        flicker_screen();
      }
      break;
    default:
      if (EEPROM.read(offset + idx) != key) {
        isValid = false;
        flicker_screen();
      }
      idx += 1;
      break;
  }
}

void handle_unlocked_keypad() {
  char key = read_keypad();
  if (key) {
    int slot = key - '0';
    if (slot >= 0 && slot <= 9) {
      int offset = get_secret_offset(slot);

      char c;
      while ((c = EEPROM.read(offset )) != 0) {
        Keyboard.print(c);
        offset += 1;
      }
    }

    switch (key) {
      case '*':
      case '#':
        break;
    }
  }
}

void loop() {
  handle_serial();

  if (isLocked) {
    handle_locked_keypad();
  } else {
    handle_unlocked_keypad();
  }
}
