#include <WiFi.h>
#include <IRCClient.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <EEPROM.h>


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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

WiFiServer server(80);


#define REPLY_TO     "NICK" // Reply only to this nick

WiFiClient wiFiClient;
IRCClient client(IRC_SERVER, IRC_PORT, wiFiClient);

const byte MODE_AP = 1;
const byte MODE_CLIENT = 2;

byte currMode = MODE_AP;

#define EEPROM_SIZE 512

void setupAP() {
  Serial.println();
  Serial.println("Configuring access point...");

  uint64_t chipid = ESP.getEfuseMac(); //The chip ID is essentially its MAC address(length: 6 bytes).
  uint16_t chip = (uint16_t)(chipid >> 32);
  static char ssid[23];

  snprintf(ssid, 23, "ESP-TOY-%04X%08X", chip, (uint32_t)chipid);

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid /*, password */);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("ACCESS POINT NAME"));
  display.println(ssid);
  display.println(F("URL"));
  display.print(F("http://"));
  display.println(myIP);
  display.display();
}

void setupClient() {
  char ssid_buf[SSID_MAX_LEN];
  getSSID(ssid_buf, sizeof ssid_buf);
  char pw_buf[PW_MAX_LEN];
  getPW(pw_buf, sizeof pw_buf);

  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(ssid_buf);
  WiFi.begin(ssid_buf, pw_buf);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  WiFi.printDiag(Serial);

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

  pinMode(LED_PIN, OUTPUT);
  pinMode(PROGRAM_SWITCH_PIN, INPUT_PULLUP);

  Serial.begin(4800);
  delay(100);

  Serial.println("IN SETUP CP1");
  EEPROM.begin(EEPROM_SIZE);

  // Start I2C Communication SDA = 5 and SCL = 4 on Wemos Lolin32 ESP32 with built-in SSD1306 OLED
  Serial.println("IN SETUP CP2");
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

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);

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

void loopClient() {
  if (!client.connected()) {
    Serial.println("Attempting IRC connection...");
    // Attempt to connect
    if (client.connect(IRC_NICKNAME, IRC_USER)) {
      Serial.println("connected");
    } else {
      Serial.println("failed... try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    return;
  }

  client.loop();
}


void loopAP() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.print("<html><head><title>ESP32 TOY Setup</title>");
            client.print("<style> label {display: inline-block: width: 25em; background-color: black; color: yellow;}</style>");

            client.print("</head>");
            client.print("<body><h1>ESP32 TOY Setup</h1>");

            client.print("<form action='' method='get'>");
            client.print("<div><label>SSID (Wifi Name)</label><input id='ssid' name='ssid'></div>");
            client.print("<div><label>Password</label><input id='pw' type='password' name='pw'></div>");
            client.print("<button type='button' onclick='save()'>Save</button></form>");

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");

            client.println("<script>");
            client.println("function save() {");
            client.println("  const encoder = new TextEncoder();");
            client.println("  const ssid = encoder.encode(document.getElementById('ssid').value);");
            client.println("  const pw = encoder.encode(document.getElementById('pw').value);");
            client.println("  const l = [...ssid, 0, ...pw, 0]");
            client.println("//alert(l);");
            client.println("fetch(`save/${l}`);");

            client.println("}");

            client.println("</script>");

            client.print("</body>");
            client.print("</html>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            Serial.println("Current line: ");
            Serial.println(currentLine);
            if (currentLine.startsWith("GET /save/")) {
              std::string::size_type l = currentLine.length();

              std::string::size_type start = 0;
              std::string::size_type fin = 0;

              // Eat the first slash
              while (currentLine[start] != '/') {
                start += 1;
              }
              start += 1;

              // Eat the second slash
              while (currentLine[start] != '/') {
                start += 1;
              }
              start += 1;

              // Read to the space
              fin = start;
              while (currentLine[fin] != ' ') {
                fin += 1;
              }

              // Force the string to end right there
              currentLine[fin] = 0;

              int idx = 0;
              char *ptr = NULL;
              ptr = strtok(&currentLine[start], ",");
              while (ptr != NULL) {
                char ch = atoi(ptr);
                if (ch == 0) {
                  break;
                }
                int offset = EEPROM_SSID_OFFSET + idx;
                EEPROM.write(offset, ch);
                EEPROM.commit();
                EEPROM.write(offset + 1, 0);
                EEPROM.commit();
                ptr = strtok(NULL, ",");
                Serial.print("SSID Saving ");
                Serial.print(ch);
                Serial.print(" at ");
                Serial.println(offset);

                EEPROM.commit();
                idx += 1;
              }

              ptr = strtok(NULL, ",");

              idx = 0;
              while (ptr != NULL) {
                char ch = atoi(ptr);
                int offset = EEPROM_PW_OFFSET + idx;
                EEPROM.write(offset, ch);
                EEPROM.commit();
                EEPROM.write(offset + 1, 0);
                EEPROM.commit();

                Serial.print("PW Saving ");
                Serial.print(ch);
                Serial.print(" at ");
                Serial.println(offset);

                ptr = strtok(NULL, ",");
                idx += 1;
              }
              EEPROM.commit();

              Serial.println("******");
            }


            char ssid_buf[SSID_MAX_LEN];
            getSSID(ssid_buf, sizeof ssid_buf);
            char pw_buf[PW_MAX_LEN];
            getPW(pw_buf, sizeof pw_buf);

            Serial.println(ssid_buf);
            Serial.println(pw_buf);

            ///Current line: GET /save/113,113,113,0,122,122,122,0 HTTP/1.1


            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }



        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_PIN, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_PIN, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
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


void callback(IRCMessage ircMessage) {
  bool cmd_good = false;
  // PRIVMSG ignoring CTCP messages
  if (ircMessage.command == "PRIVMSG" && ircMessage.text[0] != '\001') {
    String message("<" + ircMessage.nick + "> " + ircMessage.text);
    Serial.println(message);

    if (message.indexOf("LEDON") >= 0) {
      digitalWrite(LED_PIN, HIGH);
      client.sendMessage(ircMessage.nick, "Hi " + ircMessage.nick + "! LED ON");
      cmd_good = true;
    }

    if (message.indexOf("LEDOFF") >= 0) {
      digitalWrite(LED_PIN, LOW);
      client.sendMessage(ircMessage.nick, "Hi " + ircMessage.nick + "! LED OFF");
      cmd_good = true;
    }

    if (!cmd_good) {
      //    if (ircMessage.nick == REPLY_TO) {
      client.sendMessage(ircMessage.nick, "Hi " + ircMessage.nick + "! I'm your IRC bot.");
      client.sendMessage(ircMessage.nick, "Commands I know: LEDON and LEDOFF");
      //    }
    }

    return;
  }
  Serial.println(ircMessage.original);
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
