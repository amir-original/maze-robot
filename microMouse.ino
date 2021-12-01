/**
Cooperation of Multiple Robots to Solve Maze Tasks
made on November - December 2021
author: AmirRahmani
*/

#define A1 6  // Motor A pins
#define A2 5
#define B1 10 // Motor B pins
#define B2 11
int trigPinLeft=3;
int echoPinLeft=2;
int trigPinRight=8;
int echoPinRight=7;
int trigPinFront=12;
int echoPinFront=13;
long duration;
int distance;
int incomingByte = 0; // for incoming serial data

void setup() {
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps

  pinMode(trigPinFront,OUTPUT);
  pinMode(echoPinFront,INPUT);
  pinMode(trigPinRight,OUTPUT);
  pinMode(echoPinRight,INPUT);
  pinMode(trigPinLeft,OUTPUT);
  pinMode(echoPinLeft,INPUT);
  
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(B1, OUTPUT);
  pinMode(B2, OUTPUT);

  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(B1, LOW);
  digitalWrite(B2, LOW);
  
}

void loop() {
  forward();
  delay(200);
}

/**
*function of forward 
*/
void forward() {          
  analogWrite(A1, 255);
  analogWrite(A2, 0);
  analogWrite(B1, 255);
  analogWrite(B2, 0);
}
/**
*function of backward
*/
void backward() {         
  analogWrite(A1, 0);
  analogWrite(A2, 210);
  analogWrite(B1, 0);
  analogWrite(B2, 210);
}
/**
*function of stop
*/
void Stop() {              
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(B1, LOW);
  digitalWrite(B2, LOW);
}

/**
*function of turn left
*/
void turnLeft() {         
  analogWrite(A1, 0);
  analogWrite(A2, 0);
  analogWrite(B1, 70);
  analogWrite(B2, 0);
}

/**
*function of turn right
*/
void turnRight() {         
  analogWrite(A1, 70);
  analogWrite(A2, 0);
  analogWrite(B1, 0);
  analogWrite(B2, 0);
}

/**
 * get distance (cm) by ultarsonic sensor To Bar
 */
int getDistanceToBar(int trigPin,int echoPin){
  digitalWrite(trigPin,LOW);
  delayMicroseconds(2);
  digitalWrite(echoPin,LOW);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);
  duration=pulseIn(echoPin,HIGH);
  distance=duration*0.034/2;
  return distance;
}

bool isLeftClosed(){
  if(getDistanceToBar(trigPinLeft,echoPinLeft) <40){
    return true;
  }
  return false;
}

bool isRightClosed(){
  if(getDistanceToBar(trigPinRight,echoPinRight) <40){
    return true;
  }
  return false;
}

bool isFrontClosed(){
  if(getDistanceToBar(trigPinFront,echoPinFront) <40){
    return true;
  }
  return false;
}