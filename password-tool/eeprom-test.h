void writeTest() {
  ////////////////////////////

  Serial.println(F("Formatting device..."));
  OSFS::format();

  ////////////////////////////

  Serial.println(F("Storing an integer..."));
  int testInt = 999;

  OSFS::newFile("testInt", testInt);

  ////////////////////////////

  Serial.println(F("Storing a string"));

  char testStr[15] = "this is a test";

  OSFS::newFile("testStr", testStr, 15);

  ////////////////////////////

  Serial.println(F("Storing a complex data type"));

  struct complexType {
    int a;
    char b;
    uint32_t c;
  };

  complexType testCplx;
  testCplx.a = 123;
  testCplx.b = 'a';
  testCplx.c = 0xABCDEF00;

  OSFS::newFile("testCplx", testCplx);

  ////////////////////////////

  Serial.println(F("Done"));
}

void readTest(void) {

  // Var to hold the result of actions
  OSFS::result r;

  // Useful consts
  const OSFS::result noerr = OSFS::result::NO_ERROR;
  const OSFS::result notfound = OSFS::result::FILE_NOT_FOUND;

  ////////////////////////////

  Serial.println(F("Looking for testInt..."));

  int testInt;
  r = OSFS::getFile("testInt", testInt);

  if (r == notfound)
    Serial.println(F("Not found"));
  else if (r == noerr)
    Serial.println(testInt);
  else {
    Serial.print(F("Error: "));
    Serial.println( (int) r );
  }


  ////////////////////////////

  Serial.println(F("Looking for testStr..."));

  char testStr[15];
  uint16_t filePtr, fileSize;
  r = OSFS::getFileInfo("testStr", filePtr, fileSize);

  if (r == notfound)
    Serial.println(F("Not found"));
  else if (r == noerr) {

    OSFS::readNBytes(filePtr, fileSize, testStr);
    Serial.println(testStr);

  } else {
    Serial.print(F("Error: "));
    Serial.println( (int) r );
  }

  ////////////////////////////

  Serial.println(F("Looking for complex data type..."));

  struct complexType {
    int a;
    char b;
    uint32_t c;
  };

  complexType testCplx;

  r = OSFS::getFile("testCplx", testCplx);

  if (r == notfound)
    Serial.println(F("Not found"));
  else if (r == noerr) {

    Serial.println(testCplx.a);
    Serial.println(testCplx.b);
    Serial.println(testCplx.c, HEX);

  } else {
    Serial.print(F("Error: "));
    Serial.println( (int) r );
  }

  ////////////////////////////

  Serial.println(F("Done"));
}
