/* Control Speed of a DC Motor from serial monitor
   More info: http://www.ardumotive.com/how-to-drive-a-dc-motor-with-transistor.html
   Dev: Vasilakis Michalis // Date: 13/7/2015 // www.ardumotive.com */

//Transistor 'Base' pin or input pin of motor driver ic to Arduino PWM Digital Pin 3
const int motorPin = 3;
int Speed; //Variable to store Speed, by defaul 0 PWM
int flag;

void setup()
{
  pinMode(motorPin, OUTPUT); //Set pin 3 as an OUTPUT
  Serial.begin(9600); //Init serial communication
  //Print a message:
  Serial.println("Give a number from 50 to 255."); //Why minimun value 50? Because with values below 50 the motor doesn't spin ;)
  Serial.println(""); //Blank line
}

void loop()
{
  //Check if incoming data is available:
  if (Serial.available() > 0)
  {
    // If it is, we'll use parseInt() to pull out only numbers:
    Speed = Serial.parseInt();
    flag = 0;
  }

  //Valid range is from 50 to 255
  if (Speed >= 50 && Speed <= 255) {
    //Send PWM value with analogWrite to Arduino pin 3 and print a message to serial monitor
    analogWrite(motorPin, Speed);
    //Print message only once
    if (flag == 0) {
      //Print PWM value
      Serial.print("Motor spinning with ");
      Serial.print(Speed);
      Serial.println(" PWM");
      flag = 1;
    }
  }

  delay(1000);
}
//
