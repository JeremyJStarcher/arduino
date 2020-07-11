#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735

#include <SPI.h>
#include <EEPROM.h>

#include <Keypad.h>
#include <Keyboard.h>

// For the breakout board, you can use any 2 or 3 pins.
// These pins will also work for the 1.8" TFT shield.
#define TFT_CS        10
#define TFT_RST        8 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         9
#define TFT_SDA       16 // **HARDWARE PIN MOSI
#define TFT_SCL       15 // **HARDWARE PIN SCK
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

// Because these two pins are carrying power, they were chosen to pull
// from two different ports.
#define TTF_POWERPIN1 2
#define TTF_POWERPIN2 A0

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

#if  0
byte rowPins[KEY_ROWS] = {4, 5, 6, 7};
byte colPins[KEY_COLS] = { A1, A2, A3 };
#else
byte rowPins[KEY_ROWS] = {A3, A2, A1, 7};
byte colPins[KEY_COLS] = { 6, 5, 4 };
#endif

// How many total memory chucks to device the EEPROM into
const byte EEPROM_SLOTS = 16;
const byte PASSWORD_SLOT = 10;

const int slot_size = EEPROM.length() / EEPROM_SLOTS;
const int display_name_length = 10;
const int secret_length = slot_size - display_name_length;

#define SCREEN_TIMEOUT_MS ((unsigned long) 1 * 60 * 1000)

Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, KEY_ROWS, KEY_COLS);

bool isLocked = true;
bool isScreenOff = true;

const byte SCREEN_NAMES = 0;
const byte SCREEN_CMD = 1;
byte screen = SCREEN_NAMES;

void draw_screen(void) {
  if (isLocked) {
    ttf_locked_screen();
  } else {
    switch (screen) {
      case SCREEN_NAMES:
        ttf_names();
        break;
      case SCREEN_CMD:
        show_cmd();
        break;
    }
  }
}


void ttf_locked_screen(void) {
  cls();

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

void cls() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextWrap(false);
  byte fontSize = 2;
  tft.setTextSize(fontSize);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
}

void show_cmd(void) {
  cls();
  tft.setTextColor(ST77XX_YELLOW, ST77XX_BLUE);
  tft.println(F("0           "));
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.println(F("1 JIG ON    "));
  tft.setTextColor(ST77XX_YELLOW, ST77XX_BLUE);
  tft.println(F("2 JIG OFF   "));

  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.println(F("* RETURN    "));
  tft.setTextColor(ST77XX_YELLOW, ST77XX_BLUE);
  tft.println(F("# LOCK      "));
}

void ttf_names(void) {
  cls();
  byte fontSize = 2;
  tft.setTextSize(fontSize);

  for (int i = 0; i < 10; i++) {
    int text_color = i % 2 == 0 ? ST77XX_WHITE : ST77XX_WHITE;
    int bg_color = i % 2 == 0 ? ST77XX_BLUE : ST77XX_BLACK;

    tft.setTextColor(text_color, bg_color);
    tft.print(i);
    tft.print(" ");

    int offset = get_name_offset(i);

    char c;
    byte len = 0;
    while ((c = EEPROM.read(offset )) != 0) {
      tft.print(c);
      offset += 1;
      len += 1;
    }
    while (len < 9) {
      tft.print(' ');
      len += 1;
    }


    tft.println("");
  }
}

void screenOff() {
  digitalWrite(TTF_POWERPIN1, LOW);
  digitalWrite(TTF_POWERPIN2, LOW);

  pinMode(TTF_POWERPIN1, OUTPUT);
  pinMode(TTF_POWERPIN2, OUTPUT);
  isScreenOff = true;
}

void screenOn() {
  screenOff();
  digitalWrite(TTF_POWERPIN1, HIGH);
  digitalWrite(TTF_POWERPIN2, HIGH);


  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab

  isScreenOff = false;
  draw_screen();
}

void setup(void) {
  screenOn();

  Serial.begin(9600);
  Serial1.begin(9600);
  Serial1.println(F("SYSTEM LOADED"));
  Keyboard.begin();
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
  if (strlen(cmd_str) == 0) {
    return;
  }

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

  if (strcmp("help", cmd) == 0) {
    Serial1.println(F("Valid commands: "));
    Serial1.println(F("  init                  -- clears the entire system."));

    if (isLocked) {
      Serial1.println("");
      Serial1.println(F("The system is in locked mode.  The only command available"));
      Serial1.println(F("is 'init'.  Unlock using the keypad or run 'init.'"));
      Serial1.println(F(""));
      Serial1.println(F("After init, the password will be empty."));
    } else {
      Serial1.println(F("  info                  -- system information."));
      Serial1.println(F("  setname <#> <value>   -- Set display name"));
      Serial1.println(F("  setsecret <#> <value> -- Set value to type"));
      Serial1.println(F("  setunlockpin <value>  -- Set PIN"));
      Serial1.println(F("  list                  -- List information"));
    }
  }


  if (strcmp("init", cmd) == 0) {
    Serial1.println(F("Running INIT: Clearing system:"));
    for (int i = 0 ; i < EEPROM.length() ; i++) {
      EEPROM.update(i, 0);
    }
    Serial1.println(F("EEPROM CLEARED"));
  }


  if (isLocked) {
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

  if (strcmp("setunlockpin", cmd) == 0) {
    int slot = PASSWORD_SLOT;

    char *value = strings[1];
    if (strlen(value) > display_name_length + display_name_length) {
      Serial1.println(F("The password is too long"));
      return;
    }

    save_name(slot, value);
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

    Serial1.print("Setting slot ");
    Serial1.print(slot);
    Serial1.print(" to ");
    Serial1.println(value);
    save_secret(slot, value);
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
  }

  draw_screen();
}

void handle_serial() {
  static const byte CMD_LEN = 100;
  static char serial_cmd[CMD_LEN + 1];

  static byte cmd_index = 0;
  char in_char;

  while (Serial1.available() > 0) {
    in_char = Serial1.read();

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

int read_keypad() {
  unsigned long currentMillis = millis();
  static unsigned long targetMillis = 0;

  if (targetMillis == 0) {
    targetMillis = currentMillis + SCREEN_TIMEOUT_MS;
  }

  if (currentMillis > targetMillis) {
    screenOff();
  }

  char key = kpd.getKey();
  if (key) {
    targetMillis = currentMillis + SCREEN_TIMEOUT_MS;
    if (isScreenOff) {
      screenOn();
      return 0;
    }

#if 0
    tft.setCursor(0, 0);
    tft.fillScreen(ST77XX_BLACK);
    tft.print(key);
#endif

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
        screen = SCREEN_NAMES;
        draw_screen();
      } else {
        flicker_screen();
      }

      // Reset the value
      idx = 0;
      isValid = false;
      break;
    default:
      if (EEPROM.read(offset + idx) != key) {
        isValid = false;
      }
      idx += 1;
      break;
  }
}

void handle_screen1_keypad() {
  char key = read_keypad();
  if (key) {
    switch (key) {
      case '*':
        screen = 0;
        draw_screen();
        break;
      case '#':
        isLocked = true;
        draw_screen();
        break;
    }
  }
}

void handle_screen0_keypad() {
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
        screen = 1;
        draw_screen();
        break;
      case '#':
        isLocked = true;
        draw_screen();
        break;
    }
  }
}

void loop() {
  handle_serial();

  if (isLocked) {
    handle_locked_keypad();
  } else {
    switch (screen) {
      case SCREEN_NAMES:
        handle_screen0_keypad();
        break;
      case SCREEN_CMD:
        handle_screen1_keypad();
        break;
    }
  }
}
