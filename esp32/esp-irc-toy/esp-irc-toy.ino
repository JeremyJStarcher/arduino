#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "IRCClient.h"
#include "hardware.h"

char * getEpromString(int phrase, char *buffer, size_t len);
void setupAP(void);
void loopAP(void);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SSID_MAX_LEN 100
#define PW_MAX_LEN 100
#define DATA_MAX_LEN 100

#define EEPROM_MODE_OFFSET 0
#define EEPROM_DATA_OFFSET 1

#define EEPROM_SSID_NAME 0
#define EEPROM_SSID_PW 1
#define EEPROM_IRC_SERVER 2
#define EEPROM_IRC_PORT 3
#define EEPROM_IRC_NICK 4
#define EEPROM_IRC_USER 5
#define EEPROM_IRC_FULLNAME 6

const int LED_PIN = 2;

const int PROGRAM_SWITCH_PIN = 13; // FROM SCHEMATIC

//#define IRC_SERVER   "us.undernet.org"
//#define IRC_PORT     6667
//#define IRC_NICKNAME "jilly_t2"
//#define IRC_USER     "jilly_t2"
//#define IRC_FULLNAME  "Jill's RC Gizmo"


#define REPLY_TO     "NICK" // Reply only to this nick

WiFiClient wiFiClient;
IRCClient client;

const byte MODE_AP = 1;
const byte MODE_CLIENT = 2;

byte currMode = MODE_AP;
volatile bool ircRegistered = false;

#define EEPROM_SIZE 512
#define MAX_PWM 1023

void setupClient() {
  char ssid_buf[SSID_MAX_LEN];
  char pw_buf[PW_MAX_LEN];

  getEpromString(EEPROM_SSID_NAME, ssid_buf, SSID_MAX_LEN);
  getEpromString(EEPROM_SSID_PW, pw_buf, PW_MAX_LEN);

  /* Sometimes the wifi gets locked confused and won't connect
      Rebooting the device works -- hopefully re-attempting the
      connect works as well.
  */
  int attempts = 0;
  while (true) {
    attempts += 1;

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("Connecting to"));
    display.println(ssid_buf);
    display.setTextSize(2);
    display.print(F("Attempt # "));
    display.println(attempts);
    display.setTextSize(1);
    display.display();

    Serial.println("");
    Serial.print(F("Connecting to "));
    Serial.println(ssid_buf);
    WiFi.begin(ssid_buf, pw_buf);

    bool c = false;
    byte waitfor = 20;

    while (WiFi.status() != WL_CONNECTED && waitfor > 0) {
      delay(500);
      Serial.print(".");
      display.fillRect(SCREEN_WIDTH - 8, SCREEN_HEIGHT - 8, 8, 8,
                       c ? SSD1306_BLACK : SSD1306_WHITE);

      waitfor -= 1;
      c = !c;
      display.display();
    }

    if (WiFi.status() == WL_CONNECTED) {
      break;
    }
  }

  display.println("");
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  WiFi.printDiag(Serial);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(WiFi.localIP());
  display.display();

  client.setCallback(callback);
  client.setSentCallback(debugSentCallback);
}

void showBoot() {
  display.display();
  delay(2000); // Pause for 2 seconds

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("PRESS PRG BUTTON TO SET AP"));
  display.display();
  delay(2000);
}

void setup() {
  char ssid_buf[SSID_MAX_LEN];
  char pw_buf[PW_MAX_LEN];

  Serial.begin(115200);

  for (int i = 0; i < TOY_COUNT; i++) {
    pinMode(toys[i].digitalPin, OUTPUT);
    ledcSetup(i, 5000, 10);
    ledcAttachPin(toys[i].digitalPin, i);
    //digitalWrite(toys[i].digitalPin, LOW);

    Serial.print("Building ");
    Serial.print(toys[i].id);
    Serial.print(" on pin #");
    Serial.println(toys[i].digitalPin);
  }

  pinMode(PROGRAM_SWITCH_PIN, INPUT_PULLUP);

  delay(1000);

  EEPROM.begin(EEPROM_SIZE);

  client.begin(getEpromServer(),
               getEpromPort(),
               wiFiClient
              );

  // Start I2C Communication SDA = 5 and SCL = 4 on Wemos Lolin32 ESP32 with built-in SSD1306 OLED
  Wire.begin(5, 4);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  showBoot();

  int mode = digitalRead(PROGRAM_SWITCH_PIN);
  Serial.print("mode = ");
  Serial.println(mode);

  if (mode == 0) {
    currMode = MODE_AP;
  } else {
    currMode = MODE_CLIENT;
  }

  // Clear the buffer
  display.clearDisplay();

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();

  switch (currMode) {
    case MODE_CLIENT:

      char ssid_buf[SSID_MAX_LEN];
      getEpromString(EEPROM_SSID_NAME, ssid_buf, SSID_MAX_LEN);

      char pw_buf[PW_MAX_LEN];
      getEpromString(EEPROM_SSID_PW, pw_buf, PW_MAX_LEN);

      Serial.print("This was configured for ");
      Serial.print(ssid_buf);
      Serial.print(" => ");
      Serial.println(pw_buf);

      setupClient();
      break;
    case MODE_AP:
      setupAP();
      break;
  }
}

void tryIRCConnect() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Connecting to IRC"));
  //  display.println(getEpromString(EEPROM_IRC_SERVER));
  //  display.println(getEpromString(EEPROM_IRC_NICK));

  display.display();

  char buff[100];
  String nick_buf(getEpromString(EEPROM_IRC_NICK, buff, 100));

  String user_buf(getEpromString(EEPROM_IRC_USER, buff, 100));
  String fn_buf(getEpromString(EEPROM_IRC_FULLNAME, buff, 100));

  Serial.println(nick_buf);
  Serial.println(user_buf);
  Serial.println(fn_buf);

  ircRegistered = false;
  Serial.println("Attempting IRC connection...");
  // Attempt to connect
  if (client.connect(
        nick_buf,
        user_buf,
        fn_buf
      )) {
    Serial.println("connected");
  } else {
    Serial.println("failed... try again in 5 seconds");
    // Wait 5 seconds before retrying
    delay(5000);
  }
}

char *intensity_to_string(Intensity intensity, char* buf) {
  switch (intensity) {
    case intensity_none:
      strcpy(buf, "---");
      return buf;
    case intensity_high:
      strcpy(buf, "HI");
      return buf;
    case intensity_medium:
      strcpy(buf, "MED");
      return buf;
    case intensity_low:
      strcpy(buf, "LOW");
      return buf;
    default:
      strcpy(buf, "???");
      return buf;
  }
}

void showStatus() {
  char buf[100];
  for (int i = 0; i < TOY_COUNT; i++) {
    Toy toy = toys[i];
    long timeLeft = (toy.expires - millis()) / 1000;
    char buf2[100];
    intensity_to_string(toy.intensity, buf2);

    snprintf(buf, 99, "%1s %-10s %4s %2d",
             timeLeft > 0 ? "+" : " ",
             toy.id,
             buf2,
             timeLeft > 0 ? (long) timeLeft : 0
            );

    if (timeLeft > 0) {
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    }
    display.println(buf);
    display.setTextColor(SSD1306_WHITE);
  }
}

void loopClient() {
  static bool c = false;

  if (!client.connected()) {
    tryIRCConnect();
    return;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  if (ircRegistered) {
    showStatus();
  } else {
    display.println(F("Connected to IRC"));
    display.println(F("Attempting to log in."));
    display.println(F("This may wait until the nickname has timed out."));
    display.println(F("(Hope nobody else grabbed it)"));
    display.fillRect(SCREEN_WIDTH - 8, SCREEN_HEIGHT - 8, 8, 8,
                     c ? SSD1306_BLACK : SSD1306_WHITE);
  }

  display.display();
  c = !c;

  if (true) {
    for (int i = 0; i < TOY_COUNT; i++) {
      Toy toy = toys[i];
      byte digitalPin = toys[i].digitalPin;

      long timeLeft = (toy.expires - millis()) / 1000;
      if (timeLeft <= 0) {
        ledcWrite(i, 0);
        //digitalWrite(digitalPin, LOW);
      } else {
        //digitalWrite(digitalPin, HIGH);
        ledcWrite(i, toy.pwm);
      }
    }
  }

  client.loop();
}

void loop() {
  switch (currMode) {
    case MODE_CLIENT:
      loopClient();
      break;
    case MODE_AP:
      loopAP();
      break;
  }
}

void processListCommand(IRCMessage ircMessage) {
  static char buf[100];
  snprintf(buf, 99, "%4s %-10s %-20s %11s %6s", "---", "(code)", "(name)", "(intensity)", "(time)");
  client.sendMessage(ircMessage.nick, buf);

  for (int i = 0; i < TOY_COUNT; i++) {
    Toy toy = toys[i];
    long timeLeft = (toy.expires - millis()) / 1000;
    char buf2[100];
    intensity_to_string(toy.intensity, buf2);

    snprintf(buf, 99, "%4s %-10s %-20s %11s %6d",
             timeLeft > 0 ? "on" : "off",
             toy.id,
             &toy.name[0],
             buf2,
             timeLeft > 0 ? (long) timeLeft : 0
            );

    client.sendMessage(ircMessage.nick, buf);
  }
}

void processSetCommand(IRCMessage ircMessage) {
  char *setKeyword = NULL;
  char *code = NULL;
  char *intensity = NULL;
  char *duration = NULL;
  size_t toyIndex = 0;

  setKeyword = strtok(&ircMessage.text[0], " ");
  code = strtok(NULL, " ");
  intensity = strtok(NULL, " ");
  duration = strtok(NULL, " ");


  //  client.sendMessage(ircMessage.nick, setKeyword);
  //  client.sendMessage(ircMessage.nick, code);
  //  client.sendMessage(ircMessage.nick, intensity);
  //  client.sendMessage(ircMessage.nick, duration);

  if (duration == NULL) {
    client.sendMessage(ircMessage.nick, F("Missing parameters"));
    return;
  }

  unsigned short dur = atoi(duration);
  unsigned short in = atoi(intensity);

  if (dur > 60 || dur < 2) {
    client.sendMessage(ircMessage.nick, F("Duration must be between 2 and 60"));
    return;
  }

  for (toyIndex = 0; toyIndex < TOY_COUNT; toyIndex++) {
    if (toys[toyIndex].id == String(code)) {
      break;
    }
  }

  if (toyIndex == TOY_COUNT) {
    char buf[100];
    snprintf(buf, 99, "The following code is not known: '%s'.  See the 'list' command.", code);
    client.sendMessage(ircMessage.nick, buf);
    return;
  }

  toys[toyIndex].expires = millis() + (atoi(duration) * 1000);

  if (intensity[0] == 'h') {
    toys[toyIndex].intensity = intensity_high;
    toys[toyIndex].pwm = toys[toyIndex].high * MAX_PWM;
  } else if (intensity[0] == 'm') {
    toys[toyIndex].intensity = intensity_medium;
    toys[toyIndex].pwm = toys[toyIndex].medium * MAX_PWM;
  } else if (intensity[0] == 'l') {
    toys[toyIndex].intensity = intensity_low;
    toys[toyIndex].pwm = toys[toyIndex].low * MAX_PWM;
  } else if (intensity[0] == 'n') {
    toys[toyIndex].intensity = intensity_none;
    toys[toyIndex].pwm = 0;
  } else {
    toys[toyIndex].pwm = 0;
    client.sendMessage(ircMessage.nick, "Unknown intensity. Must be 'high', 'medium' or 'low'.");
    client.sendMessage(ircMessage.nick, "May also say 'h' 'm' or 'l'");
  }

  client.sendMessage(ircMessage.nick, "Command set.");

  processListCommand(ircMessage);
}

void callback(IRCMessage ircMessage) {
  bool cmd_good = false;

  if (ircMessage.command == "001") {
    ircRegistered = true;
  }

  Serial.println(ircMessage.command);
  Serial.println(ircMessage.text);

  // PRIVMSG ignoring CTCP messages
  if (ircMessage.command == "PRIVMSG" && ircMessage.text[0] != '\001') {
    String message("<" + ircMessage.nick + "> " + ircMessage.text);
    Serial.println(message);

    if (ircMessage.text.indexOf("help") == 0) {
      client.sendMessage(ircMessage.nick, "set (code) (intensity) (duration)");
      client.sendMessage(ircMessage.nick, "(code) is a code as given in the 'list' command.");
      client.sendMessage(ircMessage.nick, "(intensity) is 'high', 'medium' or 'low'.  May also use 'h' 'm' 'l'");
      client.sendMessage(ircMessage.nick, "(duration) is time, given in seconds.");
      cmd_good = true;
    }

    if (ircMessage.text.indexOf("set") == 0) {
      processSetCommand(ircMessage);
      cmd_good = true;
    }

    if (ircMessage.text.indexOf("list") == 0) {
      processListCommand(ircMessage);
      cmd_good = true;
    }

    if (!cmd_good) {
      //    if (ircMessage.nick == REPLY_TO) {
      // client.sendMessage(ircMessage.nick, "Hi " + ircMessage.nick + "! I'm your IRC bot.");
      client.sendMessage(ircMessage.nick, "Commands I know: 'list' 'help' 'set'");
      //    }
    }

    return;
  }

  /*
    [[[text *** Found your hostname]]]
    [[[orginal  :elysium.us.ix.undernet.org 433 * jilly_t2 :Nickname is already in use.]]]
    [[[prefix elysium.us.ix.undernet.or]]]
    [[[nick ]]]
    [[[user ]]]
    [[[host ]]]
    [[[command  433]]]
    [[[para * jilly_t2]]]
    [[[text Nickname is already in use.]]]
  */


  // #define AA(a, c) {Serial.print(F("[[["));Serial.print(a);Serial.print("\t");Serial.print(c);Serial.println(F("]]]"));}
#define AA(a, c)
  AA("orginal", ircMessage.original);
  AA("prefix", ircMessage.prefix);
  AA("nick", ircMessage.nick);
  AA("user", ircMessage.user);
  AA("host", ircMessage.host);
  AA("command", ircMessage.command);
  AA("para", ircMessage.parameters);
  AA("text", ircMessage.text);
}

void debugSentCallback(String data) {
  return;
  Serial.println(data);
}

char * getEpromString(int phrase, char *buffer, size_t len) {
  size_t i = 0;
  size_t idx = 0;
  int ch = -1;

  for (int ii = 0; ii <= phrase; ii++) {
    i = 0;
    while ((ch = EEPROM.read(EEPROM_DATA_OFFSET + idx)) != 0) {
      // Un-initialized memory defaults to 0xff
      if (ch == 0xff) break;

      buffer[i] = ch;
      buffer[i + 1] = 0;
      i++;
      idx += 1;
    }
    idx += 1;
  }
  return buffer;
}

char * getEpromServer() {
  static char buffer[100];
  return getEpromString(EEPROM_IRC_SERVER, buffer, 100);
}

int getEpromPort() {
  static char buffer[100];
  return atoi(getEpromString(EEPROM_IRC_PORT, buffer, 100));
}
#include "ap.h"
