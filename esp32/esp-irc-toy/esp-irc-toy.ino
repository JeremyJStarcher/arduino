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

void setupAP(void);
void loopAP(void);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define SSID_MAX_LEN 100
#define PW_MAX_LEN 100

#define EEPROM_MODE_OFFSET 0
#define EEPROM_SSID_OFFSET 1
#define EEPROM_PW_OFFSET (SSID_MAX_LEN + 1)

const int LED_PIN = 2;
const int PROGRAM_SWITCH_PIN = 16;

#define IRC_SERVER   "eu.undernet.org"
#define IRC_PORT     6667
#define IRC_NICKNAME "jilly_t2"
#define IRC_USER     "jilly_t2"
#define IRC_FULLNAME  "Jill's RC Gizmo"


#define REPLY_TO     "NICK" // Reply only to this nick

WiFiClient wiFiClient;
IRCClient client(IRC_SERVER, IRC_PORT, wiFiClient);

const byte MODE_AP = 1;
const byte MODE_CLIENT = 2;

byte currMode = MODE_AP;
volatile bool ircRegistered = false;

#define EEPROM_SIZE 512

void setupClient() {
  char ssid_buf[SSID_MAX_LEN];
  getSSID(ssid_buf, sizeof ssid_buf);
  char pw_buf[PW_MAX_LEN];
  getPW(pw_buf, sizeof pw_buf);

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
    Serial.print("Building ");
    Serial.println(toys[i].id);
  }
  pinMode(PROGRAM_SWITCH_PIN, INPUT_PULLUP);

  delay(1000);

  EEPROM.begin(EEPROM_SIZE);

  // Start I2C Communication SDA = 5 and SCL = 4 on Wemos Lolin32 ESP32 with built-in SSD1306 OLED
  Wire.begin(5, 4);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  showBoot();

  int mode = digitalRead(PROGRAM_SWITCH_PIN);
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
      getSSID(ssid_buf, sizeof ssid_buf);
      getPW(pw_buf, sizeof pw_buf);

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
  display.println(IRC_SERVER);
  display.println(IRC_NICKNAME);

  display.display();

  ircRegistered = false;
  Serial.println("Attempting IRC connection...");
  // Attempt to connect
  if (client.connect(IRC_NICKNAME, IRC_USER, IRC_FULLNAME)) {
    Serial.println("connected");
  } else {
    Serial.println("failed... try again in 5 seconds");
    // Wait 5 seconds before retrying
    delay(5000);
  }
}

void showStatus() {
  char buf[100];
  for (int i = 0; i < TOY_COUNT; i++) {
    Toy toy = toys[i];
    long timeLeft = (toy.expires - millis()) / 1000;

    snprintf(buf, 99, "%1s %-10s %3ld %2d",
             timeLeft > 0 ? "+" : " ",
             toy.id,
             toy.intensity,
             timeLeft > 0 ? (long) timeLeft : 0
            );

    display.println(buf);
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

  for (int i = 0; i < TOY_COUNT; i++) {
    Toy toy = toys[i];
    long timeLeft = (toy.expires - millis()) / 1000;
    if (timeLeft < 0) {
      digitalWrite(toy.digitalPin, LOW);
      ledcWrite(i, 0);
    } else {
      int pwm = map(toy.intensity, 0, 100, 0, 1023);
      ledcWrite(i, pwm);
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
  char buf[100];
  snprintf(buf, 99, "%4s %-10s %-20s %11s %6s", "---", "(code)", "(name)", "(intensity)", "(time)");
  client.sendMessage(ircMessage.nick, buf);

  for (int i = 0; i < TOY_COUNT; i++) {
    Toy toy = toys[i];
    long timeLeft = (toy.expires - millis()) / 1000;

    snprintf(buf, 99, "%4s %-10s %-20s %11ld %6d",
             timeLeft > 0 ? "on" : "off",
             toy.id,
             &toy.name[0],
             toy.intensity,
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

  if (dur > 60 || dur < 0) {
    client.sendMessage(ircMessage.nick, F("Duraction must be between 0 and 60"));
    return;
  }

  if (in > 100 || in < 0) {
    client.sendMessage(ircMessage.nick, F("Intensity must be between 0 and 100"));
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
  toys[toyIndex].intensity = atoi(intensity);
  client.sendMessage(ircMessage.nick, "Command set.");
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
      client.sendMessage(ircMessage.nick, "(intensity) power rating between 1 and 100");
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
  Serial.println(data);
}

void getEpromBuf(int eepromidx, char *buffer, size_t len) {
  size_t idx = 0;
  buffer[idx] = 0;

  while (true) {
    char ch = EEPROM.read(eepromidx + idx);

    if (ch == 0 || idx >= len) {
      break;
    }

    buffer[idx] = ch;
    buffer[idx + 1] = 0;
    idx += 1;
  }

}
void getSSID(char *buffer, size_t len) {
  getEpromBuf(EEPROM_SSID_OFFSET, buffer, len);
}

void getPW(char *buffer, size_t len) {
  getEpromBuf(EEPROM_PW_OFFSET, buffer, len);
}

#include "ap.h"
