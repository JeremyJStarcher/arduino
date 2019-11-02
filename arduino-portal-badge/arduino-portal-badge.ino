
#define MEM_CHECK false

#include <SPI.h>
#include <Wire.h>
#include <avr/pgmspace.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#if MEM_CHECK
#include "MemoryFree.h"
#endif

// The Jdriver extends the Adafruit_SSD1306 and gives us
// print_P and println_P to print static strings without
// having to use a temporary buffer.  Yeah for the memory
// savings.
#include "Jdriver.h"
#include "portal_logo.h"
#include "portal_data.h"
#include "glados_quotes.h"
#include "still_alive_music.h"

#define SPEAKER_PIN 8
#define OLED_RESET 4

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Jdriver display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_RESET);

// Jdriver display(OLED_RESET);


// The program is a simple state machine, with the main loop
// checking for the current state.
enum display_state_t {
  LOGO,
  ID_INFO,
  TWEET,
};

enum display_state_mode_t {
  INITIALIZE,
  RESUME
};



static display_state_t  display_state = LOGO;


void setDisplayState(int newState) {
  // Each "state" takes an optional parameter.
  // When setting the state, we call it with
  // true to say we are initializing the state.
  // I generally hate boolean arguments to functions
  // but I went with it for simplicity in this case.
  
  display_state = (display_state_t) newState;

  switch(display_state) {
  case LOGO:
    showLogo(INITIALIZE);
    break;
  case ID_INFO:
    showId(INITIALIZE);
    break;
  case TWEET:
    showTweet(INITIALIZE);
    break;
  }
}

void clearHeaderBar() {
  // Utility function:
  // Our display has the first 16 lines that are yellow rather
  // than blue.  That makes it a nice place to put "more important"
  // messages.  This clears that area for re-used.
  display.fillRect(0, 0, display.width(), charHeight*2+2, 0);
}


//************************************************************
// Code for ID_INFO 
//************************************************************
void showAlive(void) {
  // Since thre are only two messages that can be
  // shown here, I opted to use a boolean rather than
  // a more complicated counter
  static boolean firstMessage = true;
  clearHeaderBar();
  display.setCursor(0,0);
  display.setTextSize(2);
  // Reverse color
  display.setTextColor(BLACK, WHITE); 
  display.print_P(firstMessage ? statusAlive: statusStill);
  display.setTextColor(WHITE);
  firstMessage = !firstMessage;
}

void showIdData_P(unsigned const char *label, unsigned const char *txt) {
  display.setCursor(0,0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setTextWrap(true);

  display.println_P(label);
  display.println_P(txt);
}

void showId(int state) {
  const unsigned long interval = 5000;
  const unsigned long blinkInterval = 750;

  // This variable should be removed.
  // A previous version of this program blinked
  // along the bottom line and we didn't want to override
  // the message.
  static boolean allowBlink = true;
  static uint8_t currentPage = 0;
  static unsigned long previousMillis = 0; 
  static unsigned long previousBlinkMillis = 0; 
  static boolean doBlink = false;

  unsigned long currentMillis = millis();
  unsigned long timePassed = currentMillis - previousMillis;
  unsigned long blinkTimePassed = currentMillis - previousBlinkMillis;

  boolean redraw = false;

  if (state == INITIALIZE) {
    currentPage = 0;
    previousMillis = currentMillis;
    previousBlinkMillis = currentMillis;
  }

  if (blinkTimePassed > blinkInterval)
  {
    previousBlinkMillis = currentMillis;
    doBlink = !doBlink;
    redraw = true;  
  }

  // In retrospect, coming back to look at this code, I'd 
  // get rid of the `currentPage ==` checks and put in a simple
  // boolean 'changedPage' on each line.
  // Ah well.
  
  // Has the time paged to change page?
  // And are we on the previous page?
  if ((timePassed > interval * 5) && currentPage == 4) {
    allowBlink = false;
    redraw = true;
    currentPage = 5;
    setDisplayState(TWEET);
  }

  if ((timePassed > interval *4) && currentPage == 3) {
    redraw = true;
    allowBlink = true;
    currentPage = 4;
  }

  if ((timePassed > interval *3) && currentPage == 2) {
    redraw = true;
    allowBlink = true;
    currentPage = 3;
  } 

  if ((timePassed > interval *2) && currentPage == 1) {
    redraw = true;
    allowBlink = true;
    currentPage = 2;
  } 

  if ((timePassed > interval *1) && currentPage == 0) {
    redraw = true;
    allowBlink = true;
    currentPage = 1;
  } 

  if (redraw) {
    display.clearDisplay();

    if(currentPage == 4) {
      showIdData_P(employeeSecurityAccessLabel, employeeSecurityAccess);
    }
    else if(currentPage == 3) {
      showIdData_P(employeeSecurityClearanceLabel, employeeSecurityClearance);  
    } 
    else if(currentPage == 2) {
      showIdData_P(employeeDepartmentLabel, employeeDepartment);
    } 
    else if(currentPage == 1) {
      showIdData_P(employeeNameLabel, employeeName);
    }

    if (doBlink && allowBlink) {
      showAlive();
    }
    display.display();
  }
}

//************************************************************
// End code for ID_INFO 
//************************************************************


//************************************************************
// Code for LOGO
//************************************************************


void showLogo(int state) {
  static uint8_t loopMax;
  static uint8_t loopI;

  if (state == INITIALIZE) {

    uint8_t len = strlen_P((char *) logoMessage);
    display.clearDisplay();
    display.drawBitmap(
    ((display.width()-LOGO_WIDTH)/2),
    12,
    aperture_science_logo,
    LOGO_HEIGHT, LOGO_WIDTH, 1);

    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setTextWrap(false);

    loopMax = len * charWidth * 2;
    loopI = 0;
  }

  if (loopI < loopMax) {
    clearHeaderBar();
    display.setCursor(0-loopI,0);

    display.print_P(logoMessage);
    display.display();
    loopI++;
  } 
  else {
    setDisplayState(ID_INFO);
  }
}

//************************************************************
// End code for LOGO
//************************************************************


//************************************************************
// Code for TWEET
//************************************************************

void showTweet(int state) {
  // The tweet code assumes that all of the 'tweet' messages will be padded out
  // with spaces to force wrapping around to the next line.
  
  static uint8_t tweet_id;
  static int16_t offset;
  static unsigned long previousScrollMillies;
  
  const unsigned long scrollSpeed = 200;
  const uint8_t lineLength = 21;

  if (state == INITIALIZE) {
    tweet_id = random(GladosStrings);
    // Start the messages down far enough on the screen
    // that they don't run into the header bar.
    offset = +25;
    previousScrollMillies = millis();
    return;
  }  
  
  // Slow the scrolling down
  unsigned long timePassed = millis() - previousScrollMillies;
  if (!(timePassed > scrollSpeed)) {
    return;
  }
  
  previousScrollMillies = millis();
  
  const unsigned char *src = 
  (const unsigned char*)     // We are getting a pointer into PROG_MEM memory
  pgm_read_word(             // By reading a value from the list of GLADOS quotes
  glados_table               // Start at the beginning of the table
  + tweet_id                 // Look at the reference the right offset from there.
  );
  
 // The quotes begin with a special character that was to tell us if this was a tweet
 // or an email.  That never happened, we we just "eat" that character.
  src++;
  
  
  // Because the lines are all padded out, we can use the line length to get the height too!
// Math is magic.  
  int rows = (strlen_P((char *) src) / lineLength);


  int textHeight = rows * charHeight;
  int scrollMax =  display.height() - textHeight;

  display.clearDisplay();
  // Yes, this might start drawing somewhere off screen. 
  // That's fine -- the stuff we want to see will be around.
  display.setCursor(0, offset);
  
  display.setTextSize(1);
  display.println_P(src);

  // Smash the header and display the special message
  // we want there.
  clearHeaderBar();  
  display.setCursor(0,0);
  display.setTextSize(2);
  display.println_P(GladOSTweet);  
  display.display();

  offset--;

  if (-offset > scrollMax) {
    // (Note the '-offset' )
    // When the message has passed off screen
    setDisplayState(LOGO);
  }  
}


//************************************************************
// End code for TWEET
//************************************************************


void music(int state) {
  // The offset with the tune data
  static long offset;  
  
  // When did we start playing music?
  static unsigned long musicStartTime; 
  
  // Stop each note this far before the next one.
  // That allows us to hear 'breaks' in if the same note is
  // played a few times in a row. 
  const uint8_t gapBetweenNotes = 5;
  
  if (state == INITIALIZE) {
    offset = 0;
    musicStartTime = millis();
  }

  // The note frequency
  int note = (int)pgm_read_word(stillAliveNotes + offset);
  // When do we start to play this note (relative to 0)?
  unsigned long startTime = (unsigned long)pgm_read_dword(stillAliveStartTime + offset);
  // And when does the next start (again, relative to 0)?
  unsigned long nextStart = (unsigned long)pgm_read_dword(stillAliveStartTime + offset + 1);

  // That tells us how long this note is
  unsigned long dur = nextStart - startTime - gapBetweenNotes;
  
  // This note should start at this time...
  unsigned long thisNoteStartTime = musicStartTime + startTime;

  // Get the time  
  signed long t = millis() - thisNoteStartTime;
  
  // Time to play the note we are looking at?
  if (t > 0) {
    // Huzzah! Play it.
    tone(SPEAKER_PIN, note, dur);

    // Did we hit the end of the tune?
    if (offset == stillAliveNoteCount -1) {
      // Then start over.
      music(INITIALIZE);
      return;
    }
    
    offset++;
  }
}

void setup()   {                
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // init done


  Serial.begin(9600); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // Show image buffer on the display hardware.
  // Since Adafruit gave the libraries, lets go
  // ahead and give them a second worth of credit.
  display.display();
  delay(1000);

  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.setTextWrap(true);

#if MEM_CHECK
  display.setTextSize(2);
  display.println_P(freeMem);
  display.println(freeMemory());
  display.println_P(bytesMem);

  display.setTextSize(1);
  display.print(display.width());
  display.print(" x ");
  display.println(display.height());
  display.println((display.height() * display.width()) / 8);
  
  display.display();

  delay(3000);
#endif

  setDisplayState(TWEET);
  music(INITIALIZE);
}


void loop(void) {

  music(RESUME);

  switch(display_state) {
  case LOGO:
    showLogo(RESUME);
    break;
  case ID_INFO:
    showId(RESUME);
    break;
  case TWEET:
    showTweet(RESUME);
    break;
  }

}
