#include <Servo.h>

int in1Pin = 22;
int enablePin1 = 2;
int in2Pin = 24;
int in3Pin = 26;
int enablePin2 = 3;
int in4Pin = 28;

byte commands[4] = {0x00,0x00,0x00,0x00};
byte prevCommands[4] = {0x01,0x01,0x01,0x01};
// Variables will be used to determine the frequency at which the sensor readings are sent 
// to the phone, and when the last command was received.

unsigned long timer0 = 2000;  // Stores the time (in millis since execution started) 
unsigned long timer1 = 0;  // Stores the time when the last sensor reading was sent to the phone
unsigned long timer2 = 0;  // Stores the time when the last command was received from the phone

// 14 byte payload that stores the sensor readings
byte three[14] = {0xee,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xcc};

// The union allows you to declare a customized data type, in this case it can be either 
// a float or a byte array of size 4. What we need is to store a float which is 4
// bytes long and retrieve each of the 4 bytes separately.
union u_sensor0{
  byte a[4];  
  float b;
}sensor0;
union u_sensor1{
  byte c[4];  
  float d;
}sensor1;

int i = 0;
int leftSpeed = 0;
int rightSpeed = 0;
bool leftDir = true;
bool rightDir = true;
bool isNotMoving = true;

void setup()
{       
  Serial.begin(9600);
  
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(enablePin1, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);
  pinMode(enablePin2, OUTPUT);
}

void loop()
{
  if(Serial.available()==4){ 
    timer2 = millis();  //Store the time when last command was received
    memcpy(prevCommands,commands,4);  //Storing the received commands   
    commands[0] = Serial.read();  //Direction
    commands[1] = Serial.read();  //Speed
    commands[2] = Serial.read();  //Angle
    commands[3] = Serial.read();  //Lights and buttons states
    /*
     Since the last byte yields the servo's angle (between 0-180), it can never be 255. At times, the two
     previous commands pick up incorrect values for the speed and angle. Meaning that they get the direction 
     correct 100% of the time but sometimes get 255 for the speed and 255 for the angle.
     */
    if((commands[2]<=0xb4)&&((commands[0]<=0xf5)&&(commands[0]>=0xf1))){
      // Make sure that the command received involves controlling the car's motors (0xf1,0xf2,0xf3)
      if(commands[0] <= 0xf3){
        if(commands[0] == 0xf1){  // Check if the move forward command was received
          if(prevCommands[0] != 0xf1){  // Change pin state to move forward only if previous state was not move forward
              setMotor1(leftSpeed, leftDir);
              setMotor2(rightSpeed, true);
              isNotMoving = false;
          }  
        }
        else if(commands[0] == 0xf2){  //Check if the move back command was received     
          if(prevCommands[0] != 0xf2){  //Change pin state to move back only if previous state was not move back
              setMotor1(leftSpeed, leftDir);
              setMotor2(rightSpeed, false);
              isNotMoving = false;
          }
        }
        else{  //Check if the stop command was received    
          if(prevCommands[0] != 0xf3){  // Change pin state to stop only if previous state was not stop
              setMotor1(0, false);
              setMotor2(0, false);
              isNotMoving = true;
          }
        }
        // Change speed only if new speed is not equal to the previous speed
        if(prevCommands[1] != commands[1]){
          leftSpeed = commands[1] + commands[2];
          rightSpeed = commands[1] - commands[2];

          if (leftSpeed > 255) { // are we over limit?
            leftSpeed = 255;
          } else if (leftSpeed < -255) {
            leftSpeed = -255;
          }

          if (rightSpeed > 255) { // are we over limit?
            rightSpeed = 255;
          } else if (rightSpeed < -255) {
            rightSpeed = -255;
          }

         if (leftSpeed > 0){ // forward
            leftDir = true;
          } else {
            leftDir = false; // rev
          }
          if (rightSpeed > 0){ // forward
            rightDir = true;
          } else {
            rightDir = false; // rev
          }

          leftSpeed = abs(leftSpeed); // negate for math functions
          rightSpeed = abs(rightSpeed);
        }          
        // Steer only if the new angle is not equal to the previous angle
        if(prevCommands[2] != commands[2]){
          leftSpeed = commands[1] + commands[2];
          rightSpeed = commands[1] - commands[2];

          if (leftSpeed > 255) { // are we over limit?
            leftSpeed = 255;
          } else if (leftSpeed < -255) {
            leftSpeed = -255;
          }

          if (rightSpeed > 255) { // are we over limit?
            rightSpeed = 255;
          } else if (rightSpeed < -255) {
            rightSpeed = -255;
          }

         if (leftSpeed > 0){ // forward
            leftDir = true;
          } else {
            leftDir = false; // rev
          }
          if (rightSpeed > 0){ // forward
            rightDir = true;
          } else {
            rightDir = false; // rev
          }

          leftSpeed = abs(leftSpeed); // negate for math functions
          rightSpeed = abs(rightSpeed);
        }         
      }
      else if(commands[0] == 0xf5){
        if(prevCommands[0] != 0xf5){
              setMotor1(0, false);
              setMotor2(0, false);
              isNotMoving = true;
        }
      }
      else{
        // (commands[0] == 0xf4)   
      } 
      // Check the front/back lights and other toggles   
      if(prevCommands[3] != commands[3]){
        // Change the light/button states
        //               _______________________________________________
        // command[3] =  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  binary
        //               |_____|_____|_____|_____|_____|_____|_____|_____|
        // Buttons ---->  Front  Back  Horn   A     B     C     D     E   
        // Front lights
        if((bitRead(prevCommands[3],7))!=(bitRead(commands[3],7))){
          if(bitRead(commands[3],7)){
            // doSomething();
          }
          else{
            // doSomething();
          }
        }
        //Back lights
        if((bitRead(prevCommands[3],6))!=(bitRead(commands[3],6))){
          if(bitRead(commands[3],6)){
            // doSomething();
          }
          else{
            // doSomething();
          }
        }
        //Horn (using front lights to test)
        if((bitRead(prevCommands[3],5))!=(bitRead(commands[3],5))){
          if(bitRead(commands[3],5)){
            // doSomething();
          }
          else{
            // doSomething();
          }
        }
      }      
    }
    else{
      //Resetting the Serial port (clearing the buffer) in case the bytes are not being read in correct order.
      Serial.end();
      Serial.begin(9600);
    }
  }
  else{
    timer0 = millis();  //Get the current time (millis since execution started)
    if((timer0 - timer2)>400){  //Check if it has been 400ms since we received last command
      //More tan 400ms have passed since last command received, car is out of range. Therefore
      //Stop the car and turn lights off
      setMotor1(0, false);
      setMotor2(0, false);
      isNotMoving = true;
    }
    if((timer0 - timer1)>=477){  //Check if it has been 477ms since sensor reading were sent
      //Calculate the 9V's voltage by multiplying the step size by the step number (analogRead(0)) 
      //This value will be in mV, which is why it's multiplied by 0.001 to convert into Volts.
      sensor0.b = 0.0;  
      //Break the sensor0 float into four bytes for transmission
      three[1] = sensor0.a[0];
      three[2] = sensor0.a[1];
      three[3] = sensor0.a[2];
      three[4] = sensor0.a[3];
      //Get sensor 2's reading
      sensor1.d = analogRead(1);  
      //Break the sensor1 float into four bytes for transmission
      three[5] = sensor1.c[0];
      three[6] = sensor1.c[1];
      three[7] = sensor1.c[2];
      three[8] = sensor1.c[3];
      //Get the remaining reading from the analog inputs
      three[9] = map(analogRead(2),0,1023,0,255);
      three[10] = map(analogRead(3),0,1023,0,255);
      three[11] = map(analogRead(4),0,1023,0,255);
      three[12] = map(analogRead(5),0,1023,0,255); 
      //Send the six sensor readings
      Serial.write(three,14);
      //Store the time when the sensor readings were sent
      timer1 = millis();
    }
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
