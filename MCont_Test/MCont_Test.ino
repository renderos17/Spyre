#include <Servo.h>


int in1Pin = 10;
int enablePin1 = 5;
int in2Pin = 11;
int in3Pin = 12;
int enablePin2 = 6;
int in4Pin = 13;

byte val;
bool isNotMoving = true;

void setup() {
  
  Serial.begin(9600);
  
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(enablePin1, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);
  pinMode(enablePin2, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    val = Serial.read();
    Serial.println(int(val));
    motorMove(int(val));
  }
 else {
    motorMove(int(val));
  }
}

void motorMove(int x) {
  if (x == 89 || x == 90) //Stop
  {
    setMotor1(0, true);
    setMotor2(0, true);
    isNotMoving = true;
  }

  if (x == 66) //Move front
  {
    setMotor1(128, true);
    setMotor2(128, true);
    isNotMoving = false;
  }

  if (x == 78) //Move back
  {
    setMotor1(128, false);
    setMotor2(128, false);
    isNotMoving = false;
  }

  if (x == 77) //left
  {
    setMotor1(128, false);
    setMotor2(128, true);
    isNotMoving = false;
  }

  if (x == 87) //right
  {
    setMotor1(128, true);
    setMotor2(128, false);
    isNotMoving = false;
  }
}

void setMotor1(int speed, boolean reverse)
{
  analogWrite(enablePin1, speed);
  digitalWrite(in1Pin, !reverse);
  digitalWrite(in2Pin, reverse);
}

void setMotor2(int speed, boolean reverse)
{
  analogWrite(enablePin2, speed);
  digitalWrite(in3Pin, !reverse);
  digitalWrite(in4Pin, reverse);
}
