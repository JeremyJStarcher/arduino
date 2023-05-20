#include "eeprom.h"

WiFiServer server(80);

void setupAP() {
  Serial.println();
  Serial.println("Configuring access point...");

  uint64_t chipid = ESP.getEfuseMac();  //The chip ID is essentially its MAC address(length: 6 bytes).
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


String htmlEncodeEpromString(int phrase) {
  static char buff[100];
  String output(getEpromString(phrase, buff, 100));

  output.replace("&", "&amp;");
  output.replace("\"", "&quot;");
  output.replace("'", "&#39;");
  output.replace("<", "&lt;");
  output.replace(">", "&gt;");
  return output;
}

const char* getSettingsPageHtml() {
  const char* htmlResponse = R"rawliteral(
HTTP/1.1 200 OK
Content-type:text/html

<html>
<head>
  <title>ESP32 TOY Setup</title>
  <meta charset='UTF-8'>
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <style>
    body {
      background-color: navy;
      color: yellow;
    }

    .wide-input label {
      display: inline-block;
      width: 15em;
      background-color: black;
      color: yellow;
    }
    .wide-input input {
      display: inline-block;
      width: 15em;
    }
  </style>
</head>
<body>
  <h1>ESP32 TOY Setup</h1>
  <form action='' method='get'>
    <fieldset>
      <legend>WIFI Configuration</legend>
      <div class='wide-input'>
        <label for='ssid'>SSID (Wifi Name)</label>
        <input value='%s' id='ssid' name='ssid'>
      </div>
      <div class='wide-input'>
        <label for='pd'>Password</label>
        <input id='pw' value='%s' type='password' name='pw'>
      </div>
      <div>
        <label for='showPasswordCheckbox'>Show Password</label>
        <input type='checkbox' id='showPasswordCheckbox' onchange='togglePasswordVisibility()'>
      </div>
    </fieldset>
    <fieldset>
      <legend>IRC Configuration</legend>
 
      <div class='wide-input'>
        <label for='irc_server'>Server</label>
        <input id='irc_server' value='%s' name='irc_server'>
      </div>
 
      <div class='wide-input'>
        <label for='irc_port'>Port</label>
        <input id='irc_port' value='%s' name='irc_port'>
      </div>
      <div class='wide-input'>
        <label for='irc_nickname'>Nickname</label>
        <input id='irc_nickname' value='%s' name='irc_nickname'>
      </div>
      <div class='wide-input'>
        <label for='irc_user'>User</label>
        <input id='irc_user' value='%s' name='irc_user'>
      </div>
      <div class='wide-input'>
        <label for='irc_fullname'>Full Name</label>
        <input id='irc_fullname' value='%s' name='irc_fullname'>
      </div>
    </fieldset>
    <button type='button' onclick='save()'>Save</button>
  </form>
  Click <a href='/H'>here</a> to turn ON the LED.<br>
  Click <a href='/L'>here</a> to turn OFF the LED.<br>

  <script>
    function g(id) {
      const encoder = new TextEncoder();
      return encoder.encode(document.getElementById(id).value);
    }
    function save() {
      const l = [
        ...g('ssid'), 0,
        ...g('pw'), 0,
        ...g('irc_server'), 0,
        ...g('irc_port'), 0,
        ...g('irc_nickname'), 0,
        ...g('irc_user'), 0,
        ...g('irc_fullname'), 0,
        -1
      ];
      fetch(`save/${l}`);
    }

    function togglePasswordVisibility() {
      const passwordInput = document.getElementById('pw');
      if (passwordInput.type === 'password') {
        passwordInput.type = 'text';
      } else {
        passwordInput.type = 'password';
      }
    }
  </script>
</body>
</html>
)rawliteral";

  return htmlResponse;
}

void saveSettings(String currentLine) {
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
  char* ptr = NULL;
  ptr = strtok(&currentLine[start], ",");
  while (ptr != NULL) {
    signed int ch = atoi(ptr);
    if (ch == -1) {
      break;
    }
    int offset = EEPROM_DATA_OFFSET + idx;
    EEPROM.write(offset, ch);
    ptr = strtok(NULL, ",");
    Serial.print("SSID Saving ");
    Serial.print((int)ch);
    Serial.print(" ");
    Serial.print((char)ch);
    Serial.print(" at ");
    Serial.println(offset);

    idx += 1;
  }
  EEPROM.commit();

  Serial.println("******");
}

void loopAP() {
  WiFiClient client = server.available();  // listen for incoming clients

  if (client) {                     // if you get a client,
    Serial.println("New Client.");  // print a message out the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        // Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {  // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

            client.printf(
              getSettingsPageHtml(),
              htmlEncodeEpromString(EEPROM_SSID_NAME).c_str(),
              htmlEncodeEpromString(EEPROM_SSID_PW).c_str(),

              htmlEncodeEpromString(EEPROM_IRC_SERVER).c_str(),
              htmlEncodeEpromString(EEPROM_IRC_PORT).c_str(),
              htmlEncodeEpromString(EEPROM_IRC_NICK).c_str(),
              htmlEncodeEpromString(EEPROM_IRC_USER).c_str(),
              htmlEncodeEpromString(EEPROM_IRC_FULLNAME).c_str());

            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            Serial.println("Current line: ");
            Serial.println(currentLine);
            if (currentLine.startsWith("GET /save/")) {
              saveSettings(currentLine);
            }

            char ssid_buf[SSID_MAX_LEN];
            getEpromString(EEPROM_SSID_NAME, ssid_buf, SSID_MAX_LEN);

            char pw_buf[PW_MAX_LEN];
            getEpromString(EEPROM_SSID_PW, pw_buf, PW_MAX_LEN);

            Serial.println("-=-=-=-=-=-=-=-=-=-=-=-");
            Serial.println(ssid_buf);
            Serial.println(pw_buf);

            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }


        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_PIN, HIGH);  // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_PIN, LOW);  // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
