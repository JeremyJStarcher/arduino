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


String getAndSend(int phrase) {
  static char buff[100];
  String output(getEpromString(phrase, buff, 100));

  output.replace("&", "&amp;");
  output.replace("\"", "&quot;");
  output.replace("'", "&#39;");
  output.replace("<", "&lt;");
  output.replace(">", "&gt;");
  return output;
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
            client.print("<fieldset>");
            client.print("<legend>WIFI Configuration</legend>");
            client.printf("<div><label>SSID (Wifi Name)</label><input value='%s' id='ssid' name='ssid'></div>", getAndSend(EEPROM_SSID_NAME).c_str());
            client.printf("<div><label>Password</label><input id='pw' value='%s' type='password' name='pw'></div>", getAndSend(EEPROM_SSID_PW).c_str());
            client.print("</fieldset>");

            client.print("<fieldset>");
            client.print("<legend>IRC Configuration</legend>");
            client.printf("<div><label>Server</label><input id='irc_server' value='%s' name='irc_server'></div>", getAndSend(EEPROM_IRC_SERVER).c_str());
            client.printf("<div><label>Port</label><input id='irc_port' value='%s' name='irc_port'></div>", getAndSend(EEPROM_IRC_PORT).c_str());
            client.printf("<div><label>Nickname</label><input id='irc_nickname' value='%s' name='irc_nickname'></div>", getAndSend(EEPROM_IRC_NICK).c_str());
            client.printf("<div><label>User</label><input id='irc_user' value='%s' name='irc_user'></div>", getAndSend(EEPROM_IRC_USER).c_str());
            client.printf("<div><label>Full Name</label><input id='irc_fullname' value='%s' name='irc_fullname'></div>", getAndSend(EEPROM_IRC_FULLNAME).c_str());
            client.print("</fieldset>");

            client.print("<button type='button' onclick='save()'>Save</button></form>");

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");

            client.println("<script>");
            client.println("function g(id) {");
            client.println("  const encoder = new TextEncoder();");
            client.println("  return encoder.encode(document.getElementById(id).value);");
            client.println("}");
            client.println("function save() {");
            client.println("  const l = [");
            client.println("    ...g('ssid'), 0,");
            client.println("    ...g('pw'), 0,");
            client.println("    ...g('irc_server'), 0,");
            client.println("    ...g('irc_port'), 0,");
            client.println("    ...g('irc_nickname'), 0,");
            client.println("    ...g('irc_user'), 0,");
            client.println("    ...g('irc_fullname'), 0,");
            client.println("    -1");
            client.println("  ];");

            client.println("  debugger;");
            client.println("  fetch(`save/${l}`);");

            client.println("}");

            client.println("</script>");

            client.print("</body>");
            client.print("</html>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
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

            char ssid_buf[SSID_MAX_LEN];
            getEpromString(EEPROM_SSID_NAME, ssid_buf, SSID_MAX_LEN);

            char pw_buf[PW_MAX_LEN];
            getEpromString(EEPROM_SSID_PW, pw_buf, PW_MAX_LEN);

            Serial.println("-=-=-=-=-=-=-=-=-=-=-=-");
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
