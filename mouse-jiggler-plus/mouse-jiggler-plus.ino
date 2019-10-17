#include <CapacitiveSensor.h>
#include <Keyboard.h>
#include <Mouse.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

#include <U8g2lib.h>


#define USE_SSD1306
#define SSDFONT_ID 2

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#ifdef USE_SSD1306
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
U8G2LOG u8g2log;
#endif


// setup the terminal (U8G2LOG) and connect to u8g2 for automatic refresh of the display
// The size (width * height) depends on the selected font and the display

// assume 4x6 font
#if SSDFONT_ID == 1
#define U8LOG_WIDTH 32
#define U8LOG_HEIGHT 10
#endif

#if SSDFONT_ID == 2
#define U8LOG_WIDTH 26
#define U8LOG_HEIGHT 4
#endif

uint8_t u8log_buffer[U8LOG_WIDTH * U8LOG_HEIGHT];

// int RXLED = 17;  // The RX LED has a defined Arduino pin

// If this pin pulled LOW then the device will work normally.
// If it is pulled high (DEFAULT) then we enter programming mode.
// In programming mode we do NOT emit mouse and keyboard events.
// That way, you can recover from when the system decides to go
// a bit crazy.
const byte program_pin = A3;

const byte control_pin = 4;
const byte jiggle_pin = 7;
const byte darken_pin = 6;

const long touch_threshold_hi = 700;
const long touch_threshold_low = 500;

const long backlight_delay = 30 * 1000;

const long debounce_delay = 1000;
const long led_backpack_port = 0x27;
const long oled_display = 0x3C;
const long rtc_address1 = 0x50;
const long rtc_address2 = 0x68;

#define IS_DEBUG 0

typedef struct
{
  byte sensor_pin;
  byte control_pin;
} ButtonConfig;

typedef struct
{
  ButtonConfig *buttonConfig;
  CapacitiveSensor sensor;
  long last_debounce_time;
  bool is_touched;
  bool is_on;
  bool should_render;
} ButtonState;

ButtonConfig buttonConfig[] = {
  {5, control_pin},
  {darken_pin, control_pin},
  {jiggle_pin, control_pin},
  {8, control_pin}
  //  {9, control_pin}
};

const size_t NUMBER_SENSORS = sizeof(buttonConfig) / sizeof(ButtonConfig);

ButtonState *buttonState;

CapacitiveSensor pinFactory(byte control_pin, byte sensor_pin) {
  return CapacitiveSensor(control_pin, sensor_pin);
}

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

uint8_t bell[8]  = {0x4, 0xe, 0xe, 0xe, 0x1f, 0x0, 0x4};
uint8_t note[8]  = {0x2, 0x3, 0x2, 0xe, 0x1e, 0xc, 0x0};
uint8_t clock[8] = {0x0, 0xe, 0x15, 0x17, 0x11, 0xe, 0x0};
uint8_t heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};
uint8_t duck[8]  = {0x0, 0xc, 0x1d, 0xf, 0xf, 0x6, 0x0};
uint8_t check[8] = {0x0, 0x1, 0x3, 0x16, 0x1c, 0x8, 0x0};
uint8_t cross[8] = {0x0, 0x1b, 0xe, 0x4, 0xe, 0x1b, 0x0};
uint8_t retarrow[8] = {  0x1, 0x1, 0x5, 0x9, 0x1f, 0x8, 0x4};

LiquidCrystal_I2C lcd(led_backpack_port, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

RTC_DS1307 rtc;

void program_mode_stall() {
  byte pgm_state;

  int blink_rate = 150;
  while ((pgm_state = digitalRead(program_pin)) == HIGH) {
    RXLED1;
    TXLED0;
    delay(blink_rate);
    RXLED0;
    TXLED1;
    delay(blink_rate);
  }
  RXLED0;
  TXLED0;
}

void setup()
{
  Serial.begin(9600);
  Keyboard.begin();
  Mouse.begin();

  // Emergency rescue.
  // If any of the drivers crash, lets have a way to rescue us from that mess.
  pinMode(program_pin, INPUT_PULLUP);

  program_mode_stall();

  lcd.init();                      // initialize the lcd
  lcd.backlight();

  lcd.createChar(0, bell);
  lcd.createChar(1, note);
  lcd.createChar(2, clock);
  lcd.createChar(3, heart);
  lcd.createChar(4, duck);
  lcd.createChar(5, check);
  lcd.createChar(6, cross);
  lcd.createChar(7, retarrow);
  lcd.home();

  lcd.print(F("Mouse Jiggler"));
  lcd.setCursor(0, 1);
  lcd.print(F("And Locker"));
  //lcd.printByte(3);

  buttonState = (ButtonState *) malloc(sizeof(ButtonConfig) * NUMBER_SENSORS);

  for (size_t i = 0; i < NUMBER_SENSORS; i++)
  {
    ButtonState *bs = &buttonState[i];
    bs->buttonConfig = &buttonConfig[i];
    bs->sensor = CapacitiveSensor(buttonConfig[i].control_pin, buttonConfig[i].sensor_pin);
    bs->last_debounce_time = 0;
    bs->is_touched = false;
    bs->is_on = false;
    bs->should_render = true;
  }

  /* U8g2 Project: SSD1306 Test Board */
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(10, 0);
  digitalWrite(9, 0);

#ifdef USE_SSD1306
  u8g2.begin();

  u8g2.setFontMode(1);

#if SSDFONT_ID == 1
  u8g2.setFont(u8g2_font_tom_thumb_4x6_mf);  // set the font for the terminal window
#endif

#if SSDFONT_ID == 2
  u8g2.setFont(u8g2_font_cu12_tr);
#endif

  u8g2log.begin(u8g2, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);

  u8g2log.setLineHeightOffset(0); // set extra space between lines in pixel, this can be negative
  u8g2log.setRedrawMode(0);   // 0: Update screen with newline, 1: Update screen for every char

#endif
  log(F("\fSystem initialized.\n"));
  // log(F("012345678901234567901234567890123456789.\n"));

  lcd.clear();
  for (size_t i = 0; i < NUMBER_SENSORS; i++)
  {
    ButtonState *bs = &buttonState[i];
    updateLCD(bs);
  }
}

void mark_all_for_display() {
  for (size_t i = 0; i < NUMBER_SENSORS; i++)
  {
    ButtonState *bs = &buttonState[i];
    bs->should_render = true;
  }
}

void display_all() {
  for (size_t i = 0; i < NUMBER_SENSORS; i++)
  {
    ButtonState *bs = &buttonState[i];
    updateLCD(bs);
  }
}

void show_programming_screen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("** PROGRAMMING *"));
  lcd.setCursor(0, 1);
  lcd.print(F("** MODE ACTIVE *"));
}

#if IS_DEBUG==1

void print_performance(
  size_t i,
  ButtonState *bs,
  long start,
  long total1) {

  Serial.print(millis() - start);        // check on performance in milliseconds
  Serial.print("\t");                    // tab character for debug windown spacing

  Serial.print(F("Sensor# "));

  Serial.print(i);
  Serial.print("\t");

  Serial.print(F("Pin "));
  Serial.print(bs->buttonConfig->sensor_pin);
  Serial.print("\t");

  Serial.println(total1);
}
#endif


void disable_all_specials() {
  for (size_t i = 0; i < NUMBER_SENSORS; i++)
  {
    ButtonState *bs = &buttonState[i];
    bs->should_render = true;
    bs->is_on = false;
    updateLCD(bs);
    bs->should_render = false;
  }
}

void loop()
{
  long start = millis();

  static long backlight_off_time = start + backlight_delay;
  static byte backlight_current_state = HIGH;

  byte pgm_new_state = digitalRead(program_pin);
  static byte pgm_current_state = pgm_new_state == HIGH ? LOW : HIGH;

  bool keypad_active = pgm_current_state == LOW && backlight_current_state == HIGH;

  if (pgm_new_state != pgm_current_state) {
    backlight_off_time = start + backlight_delay;
    disable_all_specials();
  }

  for (size_t i = 0; i < NUMBER_SENSORS; i++)
  {
    ButtonState *bs = &buttonState[i];
    long total1 = bs->sensor.capacitiveSensor(30);

    if (pgm_current_state == LOW) {
      perform_repeated_action(bs);

      if (backlight_current_state == HIGH) {
        show_clock();
      }
    }

    if ( (millis() - bs->last_debounce_time) < debounce_delay) {
      return;
    }

    bool is_touched = total1 > touch_threshold_hi;
    bool is_released = total1 < touch_threshold_low;

    if (!bs->is_touched && is_touched) {
      bs->is_touched = true;
      bs->should_render = true;

      if (keypad_active) {
        bs->is_on = !bs->is_on;
      }

      perform_instant_action(bs);
      bs->last_debounce_time = start;

      log(bs->buttonConfig->sensor_pin);
      log(F(" Activated\n"));

#if IS_DEBUG==1
      Serial.print(bs->buttonConfig->sensor_pin);
      Serial.println(F(" Activated"));
#endif
    }

    if (bs->is_touched == true && is_released) {
      bs->is_touched = false;
      bs->should_render = true;
      bs->last_debounce_time = start;

      log(bs->buttonConfig->sensor_pin);
      log(F(" Dectivated\n"));

#if IS_DEBUG==1
      Serial.print(bs->buttonConfig->sensor_pin);
      Serial.println(" Deactivated");
#endif
    }

    if (bs->is_touched) {
      backlight_off_time = start + backlight_delay;
    }

    if (pgm_current_state == LOW) {
      if (bs->should_render) {
        updateLCD(bs);
        bs->should_render = false;
      }
    }

#if IS_DEBUG==1
    if (bs->is_touched) {
      RXLED1;
      TXLED0;
    } else {
      TXLED1;
      RXLED0;
    }

    print_performance(i, bs, start, total1);
#endif
  }

  if (pgm_new_state != pgm_current_state) {
    if (pgm_new_state == HIGH) {
      show_programming_screen();
      program_mode_stall();
    }
  }

  byte backlight_new_state = start < backlight_off_time;
  backlight_new_state = HIGH;
  
  if (backlight_new_state != backlight_current_state) {
    if (backlight_new_state == HIGH) {
      lcd.backlight();
      display_all();
      log(F("BACKLIGHT: ON\n"));
    } else {
      lcd.noBacklight();
      lcd.clear();
      log("\f");
    }
  }

  backlight_current_state = backlight_new_state;
  pgm_current_state = pgm_new_state;

#if IS_DEBUG==1
  delay(400);                             // arbitrary delay to limit data to serial port
#endif
}

void updateLCD(ButtonState *bs) {
  switch (bs->buttonConfig->sensor_pin) {
    case jiggle_pin:
      lcd.setCursor(0, 0);
      lcd.print(F("JIGGLE: "));
      lcd.print(bs->is_on ? F("ON ") : F("OFF"));
  }
}

void perform_instant_action(ButtonState *bs) {
  switch (bs->buttonConfig->sensor_pin) {
    case darken_pin:
      disable_all_specials();
      log(F("Hiding screen\n"));

      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.write(KEY_F2);
      Keyboard.release(KEY_LEFT_ALT);
      Keyboard.release(KEY_LEFT_CTRL);

      lcd.setCursor(0, 1);
      lcd.print(F("SCREEN LOCKED"));
      break;
  }
}

void perform_repeated_action(ButtonState *bs) {
  switch (bs->buttonConfig->sensor_pin) {
    case jiggle_pin:
      performJiggle(bs);
      break;
  }
}

void performJiggle(ButtonState *bs) {
  // Let the mouse pointer kind of pace back and forth bored.
  static long count = 0;
  static bool dir = false;
  const int range = 100;

  if (bs->is_on) {
    if (dir == 0) {
      count++;
    }
    if (dir == 1) {
      count--;
    }

    if (count > range || count < 0) {
      dir = !dir;
    }

    int x = dir ? 1 : -1;
    Mouse.move(x, x);
  }
}

void show_two_digits(int num) {
  if (num < 10) {
    lcd.print('0');
  }
  lcd.print(num);
}

void show_clock() {
  DateTime now = rtc.now();
  static DateTime last_timestamp = 0;

  if (last_timestamp == now.unixtime())
  {
    return;
  }

  lcd.setCursor(0, 1);

  // lcd.print(now.year(), DEC);
  // lcd.print('-');
  show_two_digits(now.month());
  lcd.print('-');
  show_two_digits(now.day());

  lcd.print("  ");
  show_two_digits(now.hour());
  lcd.print(':');
  show_two_digits(now.minute());
  lcd.print(':');
  show_two_digits(now.second());
  last_timestamp = now.unixtime();
}

void log(char* str) {
#ifdef USE_SSD1306
  u8g2log.print(str);
#else
  (void) str;
#endif
}

void log(const char* str) {
#ifdef USE_SSD1306
  u8g2log.print(str);
#else
  (void) str;
#endif
}

void log(const __FlashStringHelper* str) {
#ifdef USE_SSD1306
  u8g2log.print(str);
#else
  (void) str;
#endif
}

void log(long val) {
#ifdef USE_SSD1306
  u8g2log.print(val);
#else
  (void) val;
#endif
}

void log(byte val) {
#ifdef USE_SSD1306
  u8g2log.print(val);
#else
  (void) val;
#endif
}
