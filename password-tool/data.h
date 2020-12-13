#include <ctype.h>
#include <string.h>


#define BUFF_SIZE 500
static char BUFFER[BUFF_SIZE];

enum DataStates {reset, cmd, data};

DataStates dataState = reset;

void clearBuffer() {
  BUFFER[0] = 0;
}

void doReset() {
  clearBuffer();

  dataState = cmd;
  Serial.println(F("Ready for commands.  (Try help)"));
}

char* nextWord(char* str) {

}

void cmdHelp() {
  Serial.println(F("Valid commands"));
  Serial.println(F("name: Set the name of a key control"));
  Serial.println(F("\tname <key> <string>"));
  Serial.println(F("list: List all key controls"));
  Serial.println(F("\tlist"));
}

void cmdList() {
  OSFS::result r;

  Serial.println("List");
  char key = 0; // 0 indicates no key pressed
  for (int column = 0; column < KEYBOARD_COLS; column++)
  {
    for (int row = 0; row < KEYBOARD_ROWS; row++) // Scan all rows for a key press.
    {
      key = keys[row][column]; // Remember which key was pressed.
      char fname[10] = "_name";
      fname[0] = key;

      uint16_t filePtr, fileSize;
      r = OSFS::getFileInfo(fname, filePtr, fileSize);
      Serial.print(fname);
      Serial.print(" ");
      if (r == OSFS::result::NO_ERROR) {
        Serial.println("-file-");
      } else {
        Serial.println("-err-");
      }
    }
  }
}

void cmdName(char *token, char *split) {
  token = strtok(NULL, split);
  Serial.println(F("Setting name ..."));
  char *key = token;
  token = strtok(NULL, "");

  if (strlen(token) == 0) {
    Serial.println(F("Not enough information"));
  }

  Serial.print("key ");
  Serial.print(key);
  Serial.print(" value ");
  Serial.println(token);
}

void parseCommand() {
  Serial.println("");
  Serial.println("Parsing command");
  Serial.println(BUFFER);

  char split[2] = " ";
  char *token;

  token = strtok(BUFFER, split);

  if (strcmp(token, "help") == 0) {
    cmdHelp();
    return;
  }

  if (strcmp(token, "list") == 0) {
    cmdList();
    return;
  }

  if (strcmp(token, "name") == 0) {
    cmdName(token, split);
    return;
  }

  while (token != NULL) {
    Serial.print(":");
    Serial.println(token);
    token = strtok(NULL, split);
  }
}

void readCommand() {
  while (Serial.available()) {
    char ch = Serial.read();
    char ts[2];
    ts[0] = ch;
    ts[1] = 0;

    if (ch == '\n' || ch == '\r') {
      parseCommand();
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
    case data:
      break;
  }
}
