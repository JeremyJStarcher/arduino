#define STEPPER_PIN_1 8
#define STEPPER_PIN_2 9
#define STEPPER_PIN_3 10
#define STEPPER_PIN_4 11

int step_number = 0;

void setup() {
  pinMode(STEPPER_PIN_1, OUTPUT);
  pinMode(STEPPER_PIN_2, OUTPUT);
  pinMode(STEPPER_PIN_3, OUTPUT);
  pinMode(STEPPER_PIN_4, OUTPUT);

}

void loop() {
  oneStep(false);
  delay(3);
}

void setOneHigh(int pin) {
  Serial.println(pin);
  
  
  digitalWrite(STEPPER_PIN_1, LOW);
  digitalWrite(STEPPER_PIN_2, LOW);
  digitalWrite(STEPPER_PIN_3, LOW);
  digitalWrite(STEPPER_PIN_4, LOW);
  switch (pin) {
    case 0:
      digitalWrite(STEPPER_PIN_1, HIGH);
      break;
    case 1:
      digitalWrite(STEPPER_PIN_2, HIGH);
      break;
    case 2:
      digitalWrite(STEPPER_PIN_3, HIGH);
      break;
    case 3:
      digitalWrite(STEPPER_PIN_4, HIGH);
      break;


  }
}
void oneStep(bool dir) {
  setOneHigh(step_number);
  step_number += 1;
  if (step_number > 3) {
    step_number = 0;
  }
}
