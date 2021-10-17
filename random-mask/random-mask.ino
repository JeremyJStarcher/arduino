// SpinTimer library, https://github.com/dniklaus/spin-timer,
//   add it by using the Arduino IDE Library Manager (search for spin-timer)
#include <SpinTimer.h>

// LcdKeypad, https://github.com/dniklaus/arduino-display-lcdkeypad,
//   add it by using the Arduino IDE Library Manager (search for arduino-display-lcdkeypad)
#include <LcdKeypad.h>
#include <EEPROM.h>

int EEPROMaddress = 0;

LcdKeypad* myLcdKeypad = 0;

const int SCREEN_TIMEOUT  = 30 * 1000;
unsigned long last_press_millis = millis();
bool is_lit = false;
unsigned char m_value;
signed int random_mask = -1;

void printValue();
void updateValue();

// Implement specific LcdKeypadAdapter in order to allow receiving key press events
class MyLcdKeypadAdapter : public LcdKeypadAdapter
{
  private:
    LcdKeypad* m_lcdKeypad;

  public:
    MyLcdKeypadAdapter(LcdKeypad* lcdKeypad)
      : m_lcdKeypad(lcdKeypad)
    { }

    // Specific handleKeyChanged() method implementation - define your actions here
    void handleKeyChanged(LcdKeypad::Key newKey)
    {
      if (0 != m_lcdKeypad)
      {
        char save_m_value = m_value;
        last_press_millis = millis();
        if (!is_lit) {
          // Eat a keypress if the screen is dark.
          return;
        }
        if (LcdKeypad::UP_KEY == newKey)
        {
          m_value++;
          random_mask = -1;
        }
        else if (LcdKeypad::DOWN_KEY == newKey)
        {
          m_value--;
          random_mask = -1;
        }
        else if (LcdKeypad::SELECT_KEY == newKey)
        {
          random_mask = random(m_value) + 1;
          m_value--;
          if (m_value == 255) {
            m_value = 0;
          }
        }

        printValue();
        updateValue();
      }
    }
};

void setup()
{
  myLcdKeypad = new LcdKeypad();  // instantiate an object of the LcdKeypad class, using default parameters

  // Attach the specific LcdKeypadAdapter implementation (dependency injection)
  myLcdKeypad->attachAdapter(new MyLcdKeypadAdapter(myLcdKeypad));

  randomSeed(analogRead(0));

  m_value = EEPROM.read(EEPROMaddress);
  printValue();
}

void loop()
{
  unsigned long current_millis = millis();
  unsigned long clicks = current_millis - last_press_millis;

  is_lit = clicks <= SCREEN_TIMEOUT;

  myLcdKeypad->setBacklight(static_cast<LcdKeypad::LcdBacklightColor>(LcdKeypad::LCDBL_WHITE & (is_lit ? 0xFF : 0x00)));

  scheduleTimers();  // Get the timer(s) ticked, in particular the LcdKeypad dirver's keyPollTimer
}

void updateValue() {
  EEPROM.update(EEPROMaddress, m_value);
}

void printValue() {
  myLcdKeypad->setCursor(0, 0);
  myLcdKeypad->print("# of masks ");
  myLcdKeypad->print(m_value);
  myLcdKeypad->print("            ");

  myLcdKeypad->setCursor(0, 1);

  myLcdKeypad->print("Random: ");
  if (random_mask == -1) {
    myLcdKeypad->print("None");
  } else {
    myLcdKeypad->print(random_mask);
  }
  myLcdKeypad->print("                ");
}
