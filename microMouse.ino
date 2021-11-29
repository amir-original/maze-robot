/**
Cooperation of Multiple Robots to Solve Maze Tasks
made on November - December 2021
author: AmirRahmani
*/

#define A1 6  // Motor A pins
#define A2 5
#define B1 10 // Motor B pins
#define B2 11
int trigPin1=3;
int echoPin1=2;
int trigPin2=7;
int echoPin2=8;
int trigPin3=12;
int echoPin3=13;
long duration;
int distance;
int incomingByte = 0; // for incoming serial data

void setup() {

  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);
  Serial.begin(9600);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(B1, OUTPUT);
  pinMode(B2, OUTPUT);

  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(B1, LOW);
  digitalWrite(B2, LOW);
  
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
}

void loop() {
  forward();
  delay(200);
  Serial.println(getDistanceFromSensor(trigPin1,echoPin1));
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
 * get distance (cm) by ultarsonic sensor
 */
int getDistanceFromSensor(int trigPin,int echoPin){
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