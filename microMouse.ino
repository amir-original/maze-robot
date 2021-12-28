/**
Cooperation of Multiple Robots to Solve Maze Tasks
made on November - December 2021
author: AmirRahmani
*/

#define MA1 5  // Motor A pins
#define MA2 6
#define MB1 10 // Motor B pins
#define MB2 11

#define PWM 200
#define DST 20

/**
*initialize all ultrasonics
*/
int trigPinLeft=A5;
int echoPinLeft=A4;
int trigPinRight=A1;
int echoPinRight=A0;
int trigPinFront=A3;
int echoPinFront=A2;
long duration;
int  distance;
int role=0;
int incomingByte = 0; // for incoming serial data

void setup() {
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  pinMode(trigPinFront,OUTPUT);
  pinMode(echoPinFront,INPUT);
  pinMode(trigPinRight,OUTPUT);
  pinMode(echoPinRight,INPUT);
  pinMode(trigPinLeft,OUTPUT);
  pinMode(echoPinLeft,INPUT);
  
  pinMode(MA1, OUTPUT);
  pinMode(MA2, OUTPUT);
  pinMode(MB1, OUTPUT);
  pinMode(MB2, OUTPUT);

  analogWrite(MA1, LOW);
  analogWrite(MA2, LOW);
  analogWrite(MB1, LOW);
  analogWrite(MB2, LOW);

}

void loop() {
  /*Serial.print("Left sensor: ");
  Serial.println(leftSensor());
    Serial.print("right sensor: ");
  Serial.println(rightSensor());*/
  delay(200);
  //Serial.println(frontSensor());
  if(!isFrontClosed() && isRightClosed() && isLeftClosed()){
    forward();
    Serial.println("Move Forward.");
    if(rightSensor()>leftSensor()){
      Stop();
      delayMicroseconds(200);
      turnRight();
      Serial.println("turn Right.");
      delayMicroseconds(500);
      forward();
    }else{
      Stop();
      delayMicroseconds(200);
      turnLeft();
      Serial.println("turn Left.");
      delayMicroseconds(500);
      forward();
    }
  }
  if(isFrontClosed() && !isRightClosed() && isLeftClosed()){
    Stop();
    delay(50);
    turnRight();
    Serial.println("turn Right.");
  }
  if(isFrontClosed() && isRightClosed() && !isLeftClosed()){
    Stop();
    delay(50);
    turnLeft();
    Serial.println("turn Left.");
  }
 if(!isFrontClosed() && !isRightClosed() && isLeftClosed()){
    Stop();
    delay(50);
    turnRight();
    Serial.println("turn Right.");
    delay(150);
    forward();
  }
  if(isFrontClosed() && !isRightClosed() && !isLeftClosed()){
    Stop();
    delay(50);
    turnLeft();
    Serial.println("turn Left.");
    role=1;
  }
  if(!isFrontClosed() && isRightClosed() && !isLeftClosed()){
    Stop();
    delay(50);
    turnLeft();
    Serial.println("turnLeft.");
    delay(200);
    forward();
    Serial.println("forward.");
  }
  if(isFrontClosed() && isRightClosed() && isLeftClosed()){
    Stop();
    delay(50);
    turnRight();
    delay(2000);
    Serial.println("reverse");
  }
  if(!isFrontClosed() && !isRightClosed() && !isLeftClosed()){
    forward();
     delay(200);
     Stop();
     
     if (!(isFrontClosed() && isRightClosed() && isLeftClosed()))
        {
         Serial.println("End Of Maze");
         Stop();
        }
     else
        {
         Serial.println("FOUR WAY INTERSECTION");
         turnRight();
         delay(100);
         turnLeft();
         delay(100);
         forward();
        }
    Serial.println("End Maze.");
  }
}

/**
*move forward function
*/
void  forward() {         
  analogWrite(MA1, PWM);
  analogWrite(MA2, LOW);
  analogWrite(MB1, LOW);
  analogWrite(MB2, PWM);
}

/**
*function of backward
*/
void backward() {        
  analogWrite(MA1, HIGH);
  analogWrite(MA2, 0);
  analogWrite(MB1, HIGH);
  analogWrite(MB2, 0);
}

/**
*function of stop
*/
void Stop() {              
  analogWrite(MA1, LOW);
  analogWrite(MA2, LOW);
  analogWrite(MB1, LOW);
  analogWrite(MB2, LOW);
}

/**
*function of turn left
*/

void turnLeft() {         
  analogWrite(MA1, LOW);
  analogWrite(MA2, PWM);
  analogWrite(MB1, LOW);
  analogWrite(MB2, PWM);
}

/**
*function of turn right
*/
void turnRight() {        
  analogWrite(MA1,PWM);
  analogWrite(MA2, LOW);
  analogWrite(MB1, PWM);
  analogWrite(MB2, LOW);
}

/**
 * get distance(cm) by ultarsonic sensor
 */
int getDuration(int trigPin,int echoPin){
  digitalWrite(trigPin,LOW);
  delayMicroseconds(5);
  digitalWrite(echoPin,LOW);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);
  duration=pulseIn(echoPin,HIGH);
  return duration;
}

/**
*get distance of ultrasonic right sensor
*/
int rightSensor(){
  long duration=getDuration(trigPinRight,echoPinRight);
  return (duration/30);
}

/**
*get distance of ultrasonic left sensor
*/
int leftSensor(){
  long duration=getDuration(trigPinLeft,echoPinLeft);
  return (duration/60);
}

/**
*get distance of ultrasonic front sensor
*/
int frontSensor(){
  long duration=getDuration(trigPinFront,echoPinFront);
  return (duration/15);
}
bool isLeftClosed(){
  if(leftSensor()<DST){
     return true;
  }
  return false;
}

bool isRightClosed(){
  if(rightSensor()<DST){
    return true;
  }
  return false;
}

bool isFrontClosed(){
  if(frontSensor() <DST){
     return true;
  }
  return false;
}