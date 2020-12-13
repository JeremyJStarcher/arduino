void keyboard_begin() {
  for (int row = 0; row < KEYBOARD_ROWS; row++)
  {
    pinMode(rowPins[row], INPUT); // Set row pins as input
    digitalWrite(rowPins[row], HIGH); // turn on Pull-ups
  }
  for (int column = 0; column < KEYBOARD_COLS; column++)
  {
    pinMode(colPins[column], OUTPUT); // Set column pins as outputs for writing
    digitalWrite(colPins[column], HIGH); // Make all columns inactive
  }
}



// returns with the key pressed, or 0 if no key is pressed
char getKey()
{
  char key = 0; // 0 indicates no key pressed
  for (int column = 0; column < KEYBOARD_COLS; column++)
  {
    digitalWrite(colPins[column], LOW); // Activate the current column.
    for (int row = 0; row < KEYBOARD_ROWS; row++) // Scan all rows for a key press.
    {
      if (digitalRead(rowPins[row]) == LOW) // Is a key pressed?
      {
        delay(debounceTime); // debounce
        while (digitalRead(rowPins[row]) == LOW)
          ; // wait for key to be released
        key = keys[row][column]; // Remember which key was pressed.
      }
    }
    digitalWrite(colPins[column], HIGH); // De-activate the current column.
  }
  return key; // returns the key pressed or 0 if none
}
