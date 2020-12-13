#include <ctype.h>
#include <string.h>

static char allKeys[] = "0123456789ABCD*#";

#define BUFF_SIZE 500
static char BUFFER[BUFF_SIZE];

enum DataStates {reset, cmd, data};

DataStates dataState = reset;

char *readFile(char *fname, const char *def) {
  OSFS::result r;

  uint16_t filePtr, fileSize;
  r = OSFS::getFileInfo(fname, filePtr, fileSize);
  if (r == OSFS::result::NO_ERROR) {
    r = OSFS::getFileInfo(fname, filePtr, fileSize);

    OSFS::readNBytes(filePtr, fileSize, (byte *) BUFFER);
    return BUFFER;
  }
  strncpy(BUFFER, def, BUFF_SIZE);
  return BUFFER;
}

char *getNameFile(char key) {
  static char file[] = "_name";
  file[0] = toupper(key);
  return file;
}

char *getTextFile(char key) {
  static char file[] = "_text";
  file[0] = toupper(key);
  return file;
}

void clearBuffer() {
  BUFFER[0] = 0;
}

void doReset() {
  clearBuffer();

  dataState = cmd;
  Serial.println(F("Ready for commands.  (Try help)"));
}

void cmdHelp() {
  Serial.println(F("Valid commands"));
  Serial.println(F("list: List all key controls"));
  Serial.println(F("\tlist"));
  Serial.println("");
  Serial.println(F("name: Set the name of a key control"));
  Serial.println(F("\tname <key> <string>"));
  Serial.println("");
  Serial.println(F("text: Set the text to auto-type"));
  Serial.println(F("\ttext <key> <string>"));
  Serial.println("");
  Serial.println(F("delete: delete the given ke"));
  Serial.println(F("\tdelete <key>"));
}

void cmdList() {
  Serial.println("List");

  for (unsigned int column = 0; column < strlen(allKeys); column++)
  {
    char key = allKeys[column];
    char *fname = getNameFile(key);

    Serial.print(key);
    Serial.print(" ");

    Serial.println(readFile(fname, "-err-"));
  }
}

void cmdName(char *token, char *split) {
  token = strtok(NULL, split);
  char *key = token;
  token = strtok(NULL, "");

  if (strlen(token) == 0) {
    Serial.println(F("Not enough information"));
    return;
  }

  char *fname = getNameFile(key[0]);

  OSFS::newFile(fname, token, strlen(token)+1, true);
  redrawScreen();
}

void cmdText(char *token, char *split) {
  token = strtok(NULL, split);
  char *key = token;
  token = strtok(NULL, "");

  if (strlen(token) == 0) {
    Serial.println(F("Not enough information"));
    return;
  }

  char *fname = getTextFile(key[0]);
  Serial.print("Setting text: ");
  Serial.println(token);

  OSFS::newFile(fname, token, strlen(token)+1, true);
}

void cmdDelete(char *token, char *split) {
  token = strtok(NULL, split);
  char *key = token;
  if (strlen(key) == 0) {
    Serial.println(F("Not enough information"));
  }

  char *fname1 = getNameFile(key[0]);
  OSFS::deleteFile(fname1);

  char *fname2 = getTextFile(key[0]);
  OSFS::deleteFile(fname2);
  redrawScreen();
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

  if (strcmp(token, "text") == 0) {
    cmdText(token, split);
    return;
  }

  if (strcmp(token, "delete") == 0) {
    cmdDelete(token, split);
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
