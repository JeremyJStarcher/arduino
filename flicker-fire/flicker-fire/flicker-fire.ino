const byte Y1 = 3;
const byte Y2 = 5;
const byte Y3 = 6;
const byte Y4 = 9;
const byte Y5 = 10;
const byte R = 11;

const byte pins[] = {Y1, Y2, Y3, Y4, Y5, R};

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:

  for (int i = 0; i < 6; i++) {
    byte p = pins[i];
    pinMode(p, OUTPUT);
    digitalWrite(p, HIGH);
    Serial.println(p);
  }

  randomSeed(analogRead(0));
}


void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < 6; i++) {
    byte p = pins[i];
    analogWrite(p, random(0, 255));
  }
  analogWrite(R, random(0, 255/4));
  delay(random(40, 70)); //Limits the speed.
}
