/**
 *  Main Robot
 *  Cooperation of Multiple Robots to Solve Maze Tasks
 *  The circuit:
 *    1.Arduino Uno
 *    2.Dc Motor 100RPM
 *    3.L9110S
 *    4.Ultrasonic HC-SR04
 *    5.Nrf24l01
 *  Created At 20/12/2021 - Updated At 15/2/2022
 *  By Amirhossein Rahmani & Mohsen Rahimi
 */

 
/*Initilize WIFI Madule*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>

RF24 radio(9, 8); // (CE, CSN) for ARD Nano
#define MA1 3  // Motor A pins
#define MA2 5
#define MB1 6 // Motor B pins
#define MB2 10

#define PWM_RIGHT 175
#define PWM_LEFT 235
#define DST 20
#define DST_OBSTACLE 9

/**
*initialize all ultrasonics
*/
int trigPinLeft=A5;
int echoPinLeft=A4;
int trigPinRight=A1;
int echoPinRight=A0;
int trigPinFront=A3;
int echoPinFront=A2;
//
long duration;
int  distance;
int cell=-1;  
bool isFindCorrectPath=false;
bool deadend=false;
int send_count=0;
uint32_t path[36],two_way[36]={0,0,0,0,0,0,
                               0,0,0,0,0,0,
                               0,0,0,0,0,0,
                               0,0,0,0,0,0,
                               0,0,0,0,0,0,
                               0,0,0,0,0,0};           
/**
 * All modes that the robot must choose between that
 * mode : null,A:RL(Right and Left open),B:FR(Front and Right open),C:LF(Front and Left open)
 * curr_dir: `F`:Forward,`R`:turn Right,`L`:turn Left
 */
char mode[36],curr_dir[36]={'\0','\0','\0','\0','\0','\0',
                            '\0','\0','\0','\0','\0','\0',
                            '\0','\0','\0','\0','\0','\0',
                            '\0','\0','\0','\0','\0','\0',
                            '\0','\0','\0','\0','\0','\0',
                            '\0','\0','\0','\0','\0','\0'};

const byte address[6] = {'R','x','A','A','A'};
uint32_t correct_path[36];
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

  //setup wifi
  radio.begin();
  radio.setAutoAck(1);                
  radio.enableAckPayload();         
  radio.setRetries(5,15);
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();
}
void loop() {
  Stop();
  delay(5000);
  //just front open
  if(isLeftWall() && isRightWall() && !isFrontWall()){
    //Serial.println("forward first");
    path[++cell]=1;
    forward();
    delay(450);
    Stop();
  }

  //just right open
  else if(isLeftWall() && !isRightWall() && isFrontWall()){
    path[++cell]=2;
    turnRight();
    delay(470);
    Stop();
    delay(5000);    
    forward();    
    delay(450);
    //Serial.println("right");
  }

  //just left open 
 else if(!isLeftWall() && isRightWall() && isFrontWall()){
    path[++cell]=3;
    turnLeft();
    delay(470);
    Stop();
    delay(5000);    
    forward();
    delay(450);
    //Serial.println("left");
  }

  //left and right open
  else if(!isLeftWall() && !isRightWall() && isFrontWall()){
    int rand_=random(1,3);
    //choose random way between left and right
      if(rand_==1){
        path[++cell]=2;
        curr_dir[cell]='R';
        //Serial.println("right 1");
        turnRight();
        delay(470);
        Stop();
        delay(5000);
        forward();
        delay(450); 
      }
      if(rand_==2){
        path[++cell]=3;
        curr_dir[cell]='L';
        //Serial.println("left..");
        turnLeft();
        delay(470);
        Stop();
        delay(5000);        
        forward();
        delay(450);  
      }
       two_way[cell]=1;
       mode[cell]='A';//LR
       Stop();
       delay(70);
  }

  //left and front open
  else if(!isLeftWall() && isRightWall() && !isFrontWall()){
   int rand_=random(1,3);
    //choose random way between left and front
    if(rand_==1){
        path[++cell]=1;
        curr_dir[cell]='F';
        forward();
        delay(450);
       // Serial.println("forward..");
      }
      if(rand_==2){
        path[++cell]=3;
        curr_dir[cell]='L';
        //Serial.println("left..");
        turnLeft();
        delay(470);
        Stop();
        delay(5000);        
        forward();
        delay(450);
      }
      two_way[cell]=1;
      mode[cell]='C';//LF
      Stop();
      delay(70);
  }

  //right and front open
 else if(isLeftWall() && !isRightWall() && !isFrontWall()){
   int rand_=random(1,3);
    //choose random way between right and front
    if(rand_==1){
        path[++cell]=1;
        curr_dir[cell]='F';
        forward();
        delay(450);
        //Serial.println("forward..");
      }
      if(rand_==2){
        path[++cell]=2;
        curr_dir[cell]='R';
        //Serial.println("right 2");
        turnRight();
        delay(470);
        Stop();
        delay(5000);        
        forward();
        delay(450);
      }
      two_way[cell]=1;
      mode[cell]='B';//FR
      Stop();
      delay(70);
  }

  //Deadend left and right and front wall
  else if(isDeadend()){
    //Serial.println("Deadend");
     //turn back 180deg
    turnRight();
    delay(2000);
    Stop();
    delay(5000);
    forward();
    delay(450);   
    //Serial.println("turn back 180deg");
    deadend=true;
  }

  //turn back if is deadend
 if(deadend){
    turn_back();
    delay(1000);
  }

  //right and left and front open all dir open
 if(isAllDirOpen()){
    //Serial.println("all dir open");
    //Serial.println("forward");
    forward();
    delay(1000);
    Stop();
    path[++cell]=1;
    if(isAllDirOpen()){
      Stop();
      delay(2000);
      //Serial.println("End maze..");
      isFindCorrectPath=true;
      //End maze and send path to another robot
      copy(path,correct_path,36);
    }
  }

/**
 * after solve maze and find correct path send correct path to Fellow robot 
 */
 while(isFindCorrectPath){
    Stop();
    Serial.println("Stop Motor");
    if(send_count==0){
     send_correct_path_to_fellow_robot();
    }else{
       Serial.println("Already Sended.");
    }
    delay(400);
  }
}

void turn_back(){
  for(;two_way[cell]!=1;cell--){
    Stop();
    delay(5000);
    if(path[cell]==1){
      forward();
      delay(450);
      Stop();
      //Serial.println("turn back :forward");
    }else if(path[cell]==2){
      turnLeft();
      delay(470);
      Stop();
      delay(5000);    
      forward();
      delay(450);  
      //Serial.println("turn back: left");
    }else if(path[cell]==3){
      turnRight();
      delay(470);  
      Stop();
      delay(5000);
      forward();
      delay(450);      
      //Serial.println("turn back: right");
    }
    path[cell]=0;
    delay(5000);
    Stop();
}
   //two way
   if(two_way[cell]==1){
      if(mode[cell]=='A' && curr_dir[cell]=='L'){
      forward();
      delay(450);
      Stop();
      path[cell]=2;
      //Serial.println("two turn back: forward");
    }
    if(mode[cell]=='A' && curr_dir[cell]=='R'){
      forward();
      delay(450);
      Stop();
      path[cell]=3;
      //Serial.println("two turn back: forward");
    }
    //left and front open
    if(mode[cell]=='C' && curr_dir[cell]=='F'){
      turnRight();
      delay(470);   
      Stop();
      delay(5000);
      forward();
      delay(450); 
      path[cell]=3;
      //Serial.println("two turn back : right");
    }
    if(mode[cell]=='C' && curr_dir[cell]=='L'){
      turnLeft();
      delay(470);    
      Stop();
      delay(5000);
      forward();
      delay(450);      
      path[cell]=1;
      //Serial.println("turn back: left");
    }
    //right and front open
    if(mode[cell]=='B' && curr_dir[cell]=='F'){
      turnLeft();
      delay(470);    
      Stop();
      delay(5000); 
      forward();
      delay(450);
      path[cell]=2;
      //Serial.println("two turn back: left");
    }
    if(mode[cell]=='B' && curr_dir[cell]=='R'){
      turnRight();
      delay(470);    
      Stop();
      delay(5000);
      forward();
      delay(450);  
      path[cell]=1;
      //Serial.println("two turn back: right");
    }
   }
   Stop();
   delay(300);
   deadend=false;
   exit;
}

/**
 * Adjust speed and distance from obstacles
 */
void control_speed(int speed_left,int speed_right){
  analogWrite(MA1, speed_left);
  analogWrite(MA2, 0);
  analogWrite(MB1, speed_right);
  analogWrite(MB2, 0);
}

void distance_adjustment(int left_dst,int right_dst){
if(leftSensor() < left_dst){             
    control_speed(50,200);
	  delay(95);
    Stop();
    delay(1000);      
  }   

  if(rightSensor() < right_dst){
    control_speed(200,50);
    delay(95);
    Stop();
    delay(1000);       
  }
}
void  forward() {          //function of forward 
  distance_adjustment(DST_OBSTACLE,DST_OBSTACLE)
  analogWrite(MA1, LOW);
  analogWrite(MA2, PWM_LEFT);
  analogWrite(MB1, LOW);
  analogWrite(MB2, PWM_RIGHT);
}

void backward() {         //function of backward
  analogWrite(MA1, HIGH);
  analogWrite(MA2, 0);
  analogWrite(MB1, HIGH);
  analogWrite(MB2, 0);
}

void Stop() {              //function of stop
  analogWrite(MA1, LOW);
  analogWrite(MA2, LOW);
  analogWrite(MB1, LOW);
  analogWrite(MB2, LOW);
}

void turnLeft() {         //function of turn left
  analogWrite(MA1, 195);
  analogWrite(MA2, LOW);
  analogWrite(MB1, LOW);
  analogWrite(MB2, 215);
}

void turnRight() {         //function of turn right
  analogWrite(MA1, LOW);
  analogWrite(MA2, 195);
  analogWrite(MB1, 215);
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

int rightSensor(){
  long duration=getDuration(trigPinRight,echoPinRight);
  return (duration*0.034/2);
}
int leftSensor(){
  long duration=getDuration(trigPinLeft,echoPinLeft);
  return (duration*0.034/2);
}
int frontSensor(){
  long duration=getDuration(trigPinFront,echoPinFront);
  return (duration*0.034/2);
}
bool isLeftWall(){
  if(leftSensor()<DST){
     return true;
  }else{
    return false;
  } 
}

bool isRightWall(){
  if(rightSensor()<DST){
    return true;
  }
  return false;
}

bool isFrontWall(){
  if(frontSensor() < DST){
     return true;
  }
  return false;
}


bool isAllDirOpen(){
  if(!isLeftWall() && !isRightWall() && !isFrontWall())
    return true;
   else
    return false; 
}

bool isDeadend(){
  if(isFrontWall() && isRightWall() && isLeftWall()){
    return true;
  }else{          
    return false;
  }
}

void send_correct_path_to_fellow_robot(){
  int data;
  Serial.println("Sending...");
  for (int j = 0; j < 32; j++) {
      data=correct_path[j];
      radio.write(&data, sizeof(data));   
      Serial.println(data);    
      delay(1000);
   }
   Serial.println("sended successed");
   send_count=1;
}

// Function to copy 'len' elements from 'src' to 'dst'
void copy(uint32_t* src, uint32_t* dst, int len) {
    memcpy(dst, src, sizeof(src[0])*len);
}