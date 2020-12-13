#include <ctype.h>

#define BUFF_SIZE 50
static char BUFFER[BUFF_SIZE];

enum DataStates {reset, cmd, data};

DataStates dataState = reset;

void clearBuffer() {
  BUFFER[0] = 0;

}

void doReset() {
  clearBuffer();

  dataState = cmd;
  Serial.println(F("Ready for commands."));
}

void readCommand() {
  while (Serial.available()) {
    char ch = Serial.read();
    char ts[2];
    ts[0] = tolower(ch);
    ts[1] = 0;

    if (ch == '\n' || ch == '\r') {
      Serial.println("");
      Serial.println(BUFFER);
      doReset();
    } else {
      strncat(BUFFER, ts, BUFF_SIZE - 1);
    }
  }
}

void data_loop() {

  switch (dataState) {
    case reset:
      doReset();
      break;
    case cmd:
      readCommand();
      break;
  }
}
