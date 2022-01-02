#define MA1 5  // Motor A pins
#define MA2 6
#define MB1 10 // Motor B pins
#define MB2 11

#define PWM_RIGHT 150
#define PWM_LEFT 155
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
int incomingByte = 0; // for incoming serial data
int role=-1;
int path[36],two_way[36]={0,0,0,0,0,0,
              0,0,0,0,0,0,
              0,0,0,0,0,0,
              0,0,0,0,0,0,
              0,0,0,0,0,0,
              0,0,0,0,0,0};             
bool isFindCorrectPath=false;
char curr_dir='Y';
/**
 * All modes that the robot must choose between that
 * state : null,A:RL,B:FR,C:LF
 */
char mode='\0';
/**
 * 'F': forward
 * 'R': turn Right
 * 'L': turn left
 * 'B'  return
 */
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
  // put your main code here, to run repeatedly: 
  if(isFindCorrectPath){
  //finsh end maze and stop run
    Stop();
    Serial.println("Stop Run so End Maze");
    delay(8000);
  }else if(!isFindCorrectPath){
  forward;
  Serial.println("forward");
  delay(5000);
  Stop();
  delay(1000);
  //just front open
  if(isLeftWall() && isRightWall() && !isFrontWall()){
    path[++role]=1;
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
    mode='A';//LR
    //choose random way between left and right
      if(rand_==1){
        turnRight();
        path[++role]=2;
        curr_dir='R';
        Serial.println("right 1");
      }
      if(rand_==2){
        turnLeft();
        path[++role]=3;
        curr_dir='L';
         Serial.println("left..");
      }
       two_way[role]=1;
  }
  //left and front open
  if(!isLeftWall() && isRightWall() && !isFrontWall()){
   int rand_=random(1,3);
   mode='C';//LF
    //choose random way between left and front
    if(rand_==1){
        forward();
        path[++role]=1;
        curr_dir='F';
         Serial.println("forward..");
      }
      if(rand_==2){
        turnLeft();
        path[++role]=3;
        curr_dir='L';
         Serial.println("left..");
      }
      two_way[role]=1;
  }
  //right and front open
  if(isLeftWall() && !isRightWall() && !isFrontWall()){
   int rand_=random(1,3);
       mode='B';
    //choose random way between right and front
    if(rand_==1){
        forward();
        path[++role]=1;
        curr_dir='F';
        Serial.println("forward..");
      }
      if(rand_==2){
        turnRight();
        path[++role]=2;
        curr_dir='R';
        Serial.println("right 2");
      }
      two_way[role]=1;
  }
  //Deadend left and right and front wall
  if(isDeadend()){
    Serial.println("Deadend");
    turnRight();
    //turn back 180deg
    delay(2000);
    Stop();
    //turn back
    turn_back();
  }

  //right and left and front open all dir open
  if(isAllDirOpen()){
    Serial.println("all dir open");
    forward();
    delay(1000);
    Stop();
    path[++role]=1;
    if(isAllDirOpen()){
      Stop();
      isFindCorrectPath=true;
      delay(1000);
      Serial.println("End maze..");
      //End maze and send path to another robot
    }
  }
 }
}

void turn_back(){
  for(;two_way[role]!=1;role--){
    path[role]=0;
    if(path[role]==1){
      forward();
      delay(1000);
      Stop();
    }else if(path[role]==2){
      turnLeft();
    }else if(path[role]==3){
      turnRight();
    }
    //left and right open
    if(mode=='A' && curr_dir=='L'){
      forward();
      delay(1000);
      Stop();
      path[role]=2;
    }
    if(mode=='A' && curr_dir=='R'){
      forward();
      delay(1000);
      Stop();
      path[role]=3;
    }
    //left and front open
    if(mode=='C' && curr_dir=='F'){
      turnRight();
      path[role]=3;
    }
    if(mode=='C' && curr_dir=='L'){
      turnLeft();
      path[role]=1;
    }
    //right and front open
    if(mode=='B' && curr_dir=='F'){
      turnLeft();
      path[role]=2;
    }
    if(mode=='B' && curr_dir=='R'){
      turnRight();
      path[role]=1;
    }
  }
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
  return (duration/20);
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
  if(frontSensor() <DST){
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
  if(!isDeadend())
    return true;
   else
    return false; 
}
bool isDeadend(){
  if(isLeftWall() && isRightWall() && isFrontWall()){
    return true;
  }else{
    return false;
  }
}
