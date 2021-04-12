/**
   WiFiManager advanced demo, contains advanced configurartion options
   Implements TRIGGEN_PIN button press, press for ondemand configportal, hold
   for 3 seconds for reset settings.
*/
#include <ESP32Ping.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN 0x10
#endif

#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 23

#define TFT_BL 4 // Display backlight control pin

#define SCREEN_X 135
#define SCREEN_Y 240

TFT_eSPI tft = TFT_eSPI(SCREEN_X, SCREEN_Y); // Invoke custom library

SPIClass SDSPI(HSPI);

#define TRIGGER_PIN 35
#define AP_NAME "WifiManger"
#define AP_PASSWORD "ncc-1701"
#define SUCCESS_DELAY (1000 * 60 * 1)
#define FAIL_DELAY (1000 * 10)

WiFiManager wm;                    // global wm instance
WiFiManagerParameter custom_field; // global param ( for non blocking w params )

void setupTft() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(0, 0);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1);

  if (TFT_BL > 0) {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);
  }

  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(TFT_CYAN);
  tft.drawString(F("Booting"), tft.width() / 2, (tft.height() / 2) * 1);
}

void setup() {
  setupTft();

  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(3000);
  Serial.println("\n Starting");

  pinMode(TRIGGER_PIN, INPUT);

  int customFieldLength = 40;

  // new (&custom_field) WiFiManagerParameter("customfieldid", "Custom Field
  // Label", "Custom Field Value", customFieldLength,"placeholder=\"Custom Field
  // Placeholder\"");

  // test custom html input type(checkbox)
  // new (&custom_field) WiFiManagerParameter("customfieldid", "Custom Field
  // Label", "Custom Field Value", customFieldLength,"placeholder=\"Custom Field
  // Placeholder\" type=\"checkbox\""); // custom html type

  // test custom html(radio)
  const char *custom_radio_str =
    "<br/><label for='customfieldid'>Custom Field Label</label><input "
    "type='radio' name='customfieldid' value='1' checked> One<br><input "
    "type='radio' name='customfieldid' value='2'> Two<br><input type='radio' "
    "name='customfieldid' value='3'> Three";
  new (&custom_field)
  WiFiManagerParameter(custom_radio_str); // custom html input

  wm.addParameter(&custom_field);
  wm.setSaveParamsCallback(saveParamCallback);

  // custom menu via array or vector
  //
  // menu tokens,
  // "wifi","wifinoscan","info","param","close","sep","erase","restart","exit"
  // (sep is seperator) (if param is in menu, params will not show up in wifi
  // page!) const char* menu[] = {"wifi","info","param","sep","restart","exit"};
  // wm.setMenu(menu,6);
  std::vector<const char *> menu = {"wifi", "info",    "param",
                                    "sep",  "restart", "exit"
                                   };
  wm.setMenu(menu);

  // set dark theme
  wm.setClass("invert");

  // set static ip
  // wm.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1),
  // IPAddress(255,255,255,0)); // set static ip,gw,sn
  // wm.setShowStaticFields(true); // force show static ip fields
  // wm.setShowDnsFields(true);    // force show dns field always

  // wm.setConnectTimeout(20); // how long to try to connect for before
  // continuing
  wm.setConfigPortalTimeout(30); // auto close configportal after n seconds
  // wm.setCaptivePortalEnable(false); // disable captive portal redirection
  // wm.setAPClientCheck(true); // avoid timeout if client connected to softap

  // wifi scan settings
  // wm.setRemoveDuplicateAPs(false); // do not remove duplicate ap names (true)
  // wm.setMinimumSignalQuality(20);  // set min RSSI (percentage) to show in
  // scans, null = 8% wm.setShowInfoErase(false);      // do not show erase
  // button on info page wm.setScanDispPerc(true);       // show RSSI as
  // percentage not graph icons

  // wm.setBreakAfterConfig(true);   // always exit configportal even if wifi
  // save fails

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);

  tft.setTextSize(2);
  tft.drawString("CONFIGURE MODE", tft.width() / 2, (tft.height() / 4) * 1);
  tft.drawString(AP_NAME, tft.width() / 2, (tft.height() / 4) * 2);
  tft.drawString(AP_PASSWORD, tft.width() / 2, (tft.height() / 4) * 3);

  bool res;
  res = wm.autoConnect(AP_NAME, AP_PASSWORD); // password protected ap

  if (!res) {
    tft.setTextColor(TFT_RED);
    tft.drawString(F("NO CONNECTION"), tft.width() / 2, (tft.height() / 4) * 4);
    Serial.println("Failed to connect or hit timeout");
    // ESP.restart();
  } else {
    tft.setTextColor(TFT_GREEN);
    tft.drawString(F("CONNECTION"), tft.width() / 2, (tft.height() / 4) * 4);

    // if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
  }
}

void checkButton() {
  bool val = LOW;

  // check for button press
  if (digitalRead(TRIGGER_PIN) == val) {
    // poor mans debounce/press-hold, code not ideal for production
    delay(50);
    if (digitalRead(TRIGGER_PIN) == val) {
      Serial.println("Button Pressed");
      // still holding button for 3000 ms, reset settings, code not ideaa for
      // production
      delay(3000); // reset delay hold
      if (digitalRead(TRIGGER_PIN) == val) {
        Serial.println("Button Held");
        Serial.println("Erasing Config, restarting");
        wm.resetSettings();
        ESP.restart();
      }

      // start portal w delay
      Serial.println("Starting config portal");
      wm.setConfigPortalTimeout(120);

      if (!wm.startConfigPortal(AP_NAME, AP_PASSWORD)) {
        Serial.println("failed to connect or hit timeout");
        delay(3000);
        // ESP.restart();
      } else {
        // if you get here you have connected to the WiFi
        Serial.println("connected...yeey :)");
      }
    }
  }
}

String getParam(String name) {
  // read parameter from server, for customhmtl input
  String value;
  if (wm.server->hasArg(name)) {
    value = wm.server->arg(name);
  }
  return value;
}

void saveParamCallback() {
  Serial.println("[CALLBACK] saveParamCallback fired");
  Serial.println("PARAM customfieldid = " + getParam("customfieldid"));
}

void drawBorder(uint32_t color, int r) {
  for (int i = 0; i < r; i++) {
    tft.drawRect(0 + i, 0 + i, tft.width() - (i * 2) - 1, tft.height() - (i * 2) - 1,
                 color);
  }
}

void loopScan() {
  //
  // #define SUCCESS_DELAY (1000 * 60 * 1)
  // #define FAIL_DELAY =  (1000 * 10)

  static uint16_t lastColor = TFT_BLACK;
  static int interval = 0;
  static unsigned long previousMillis = 0;
  static byte lastPercent = 1;
  static int lastWidth = -1;
  static bool refreshScreen = false;

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis < interval) {
    int percent = 100 * (((float) currentMillis - (float) previousMillis) / (float) interval);
    float fpercent = (((float) currentMillis - (float) previousMillis) / (float) interval);
    int barWidth = tft.width() * fpercent;
    int barHeight = 24;

    if ((percent != lastPercent) || barWidth != lastWidth) {
      if (refreshScreen) {
        tft.fillScreen(lastColor);
        tft.setTextColor(TFT_BLACK);
        tft.fillRect(0, 0, tft.width(), barHeight, TFT_BLACK);
        refreshScreen = false;
      }

      String ps = String(percent) + "%";
      int tx = tft.width() / 2;
      int ty = (tft.height() / 4) * 0;

      tft.fillRect(0, 0, barWidth, barHeight, TFT_WHITE);
      tft.fillRect(barWidth, 0, tft.width() - barWidth, barHeight, TFT_BLACK);

      tft.setTextColor(TFT_DARKGREY);
      tft.setTextSize(3);
      tft.drawString(ps, tx, ty);

      lastPercent = percent;
      lastWidth = barWidth;
    }
    return;
  }

  tft.fillScreen(lastColor);
  bool success = Ping.ping("www.google.com", 3);

  if (success) {
    lastColor = TFT_GREEN;
    tft.fillScreen(lastColor);
    interval = SUCCESS_DELAY;
  } else {
    lastColor = TFT_RED;
    tft.fillScreen(lastColor);
    interval = FAIL_DELAY;
  }
  refreshScreen = true;
  // Get a new value because we want next ping to happen
  // and a time offset from the first ping.
  // If we wanted them to happen on a regular heartbeat
  // regardless of how long the last ping took, we'd set
  // to currentMillis.
  previousMillis = millis();
}

void loop() {
  checkButton();
  loopScan();
}
