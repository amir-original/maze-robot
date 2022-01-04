/*Initilize WIFI Madule*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>

RF24 radio(9, 8); // (CE, CSN) for ARD Nano
#define MA1 5  // Motor A pins
#define MA2 6
#define MB1 10 // Motor B pins
#define MB2 11

#define PWM_RIGHT 200
#define PWM_LEFT 120
#define DST 11

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
int incomingByte = 0; // for incoming serial data
int role=-1;  
bool isFindCorrectPath=false;
bool deadend=false;
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
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();
}
void loop() {
  Serial.println("array path: ");
  for(int i=role;i>0;i--){
   Serial.print(path[i]);
   Serial.println(" , ");
   delay(1000);
  }
  Stop();
  delay(5000);

  //just front open
  if(isLeftWall() && isRightWall() && !isFrontWall()){
    forward();
    path[++role]=1;
    Serial.println("forward first");
    delay(2000);
    Stop();
    if(leftSensor() > rightSensor()){
      turnLeft();
      delay(300);
    }else{
      turnRight();
      delay(100);
    }
  }

  //just right open
  if(isLeftWall() && !isRightWall() && isFrontWall()){
    path[++role]=2;
    turnRight();
    Serial.println("right");
  }

  //just left open 
  if(!isLeftWall() && isRightWall() && isFrontWall()){
    path[++role]=3;
    turnLeft();
     Serial.println("left");
  }

  //left and right open
  if(!isLeftWall() && !isRightWall() && isFrontWall()){
    int rand_=random(1,3);
    //choose random way between left and right
      if(rand_==1){
        turnRight();
        path[++role]=2;
        curr_dir[role]='R';
        Serial.println("right 1");
      }
      if(rand_==2){
        turnLeft();
        path[++role]=3;
        curr_dir[role]='L';
        Serial.println("left..");
      }
       two_way[role]=1;
       mode[role]='A';//LR
       Stop();
       delay(70);
  }

  //left and front open
  if(!isLeftWall() && isRightWall() && !isFrontWall()){
   int rand_=random(1,3);
    //choose random way between left and front
    if(rand_==1){
        forward();
        path[++role]=1;
        curr_dir[role]='F';
         Serial.println("forward..");
      }
      if(rand_==2){
        turnLeft();
        path[++role]=3;
        curr_dir[role]='L';
         Serial.println("left..");
      }
      two_way[role]=1;
      mode[role]='C';//LF
      Stop();
      delay(70);
  }

  //right and front open
  if(isLeftWall() && !isRightWall() && !isFrontWall()){
   int rand_=random(1,3);
    //choose random way between right and front
    if(rand_==1){
        forward();
        path[++role]=1;
        curr_dir[role]='F';
        Serial.println("forward..");
      }
      if(rand_==2){
        turnRight();
        path[++role]=2;
        curr_dir[role]='R';
        Serial.println("right 2");
      }
      two_way[role]=1;
      mode[role]='B';
      Stop();
      delay(70);
  }

  //Deadend left and right and front wall
  if(isDeadend()){
    Serial.println("Deadend");
    turnRight();
    //turn back 180deg
    delay(2000);
    Serial.println("turn back 180deg");
    Stop();
    deadend=true;
  }

  //turn back if is deadend
  if(deadend){
    turn_back();
    delay(1000);
  }

  //right and left and front open all dir open
  if(isAllDirOpen()){
    Serial.println("all dir open");
    Serial.println("forward");
    forward();
    delay(4000);
    Stop();
    path[++role]=1;
    if(isAllDirOpen()){
      Stop();
      delay(2000);
      Serial.println("End maze..");
      isFindCorrectPath=true;
      //End maze and send path to another robot
      copy(path,correct_path,36);
    }
  }

/**
 * after solve maze and find correct path send correct path to Fellow robot 
 */
  if(isFindCorrectPath){
    send_correct_path_to_fellow_robot();
  }

}

void turn_back(){
  for(;two_way[role]!=1;role--){
    if(path[role]==1){
      forward();
      Serial.println("turn back :forward");
      delay(2000);
      Stop();
    }else if(path[role]==2){
      turnLeft();
      Serial.println("turn back: left");
    }else if(path[role]==3){
      turnRight();
      Serial.println("turn back: right");
    }
    path[role]=0;
    delay(5000);
    Stop();
}
   //two way
   if(two_way[role]==1){
      if(mode[role]=='A' && curr_dir[role]=='L'){
      forward();
      Serial.println("two turn back: forward");
      delay(1000);
      Stop();
      path[role]=2;
    }
    if(mode[role]=='A' && curr_dir[role]=='R'){
      forward();
      Serial.println("two turn back: forward");
      delay(1000);
      Stop();
      path[role]=3;
    }
    //left and front open
    if(mode[role]=='C' && curr_dir[role]=='F'){
      turnRight();
      path[role]=3;
      Serial.println("two turn back : right");
    }
    if(mode[role]=='C' && curr_dir[role]=='L'){
      turnLeft();
      Serial.println("turn back: left");
      path[role]=1;
    }
    //right and front open
    if(mode[role]=='B' && curr_dir[role]=='F'){
      turnLeft();
      Serial.println("two turn back: left");
      path[role]=2;
    }
    if(mode[role]=='B' && curr_dir[role]=='R'){
      turnRight();
      Serial.println("two turn back: right");
      path[role]=1;
    }
   }
   Stop();
   delay(180);
   deadend=false;
   exit;
}


void  forward() {          //function of forward 
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
  analogWrite(MA1, PWM_LEFT);
  analogWrite(MA2, LOW);
  analogWrite(MB1, LOW);
  analogWrite(MB2, PWM_RIGHT);
}

void turnRight() {         //function of turn right
  analogWrite(MA1, LOW);
  analogWrite(MA2, PWM_LEFT);
  analogWrite(MB1, PWM_RIGHT);
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
  return (duration/40);
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

bool isWall(){
  if(isFrontWall)
    return true;
  else if(isRightWall())
    return true;
  else if (isLeftWall())
    return true;
  else
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

bool send_correct_path_to_fellow_robot(){
  Serial.println("Sending...");
  int result=radio.write(&correct_path, sizeof(correct_path));
  if(!result){
   Serial.println("oops!, send failed!!");
   return false;
  }
   Serial.println("sended successed");
   return true;
  delay(200);
}

// Function to copy 'len' elements from 'src' to 'dst'
void copy(uint32_t* src, uint32_t* dst, int len) {
    memcpy(dst, src, sizeof(src[0])*len);
}
