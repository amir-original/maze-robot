
/**
 *  Fellow Robot
 *  Cooperation of Multiple Robots to Solve Maze Tasks
 *  The circuit:
 *    1.Arduino Uno
 *    2.Dc Motor 100RPM
 *    3.L9110S
 *    4.Ultrasonic HC-SR04
 *    5.Nrf24l01
 *  Created At 20/12/2021
 *  By Amirhossein Rahmani & Mohsen Rahimi
 */

/*Initilize WIFI Madule*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>

RF24 radio(9, 8); // (CE, CSN) for ARD Nano
const byte address[6] = {'R','x','A','A','A'};
#define MA1 5  // Motor A pins
#define MA2 6
#define MB1 10 // Motor B pins
#define MB2 11

#define PWM_RIGHT 200
#define PWM_LEFT 120

bool start_=false;
bool is_read_data=false;
int j=-1;
int path[32];
int data;
int count=0;

/**
 * path=1 --> move forward
 * path=2 --> turn right
 * path=3 -->turn left
 */
void setup() {
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  pinMode(MA1, OUTPUT);
  pinMode(MA2, OUTPUT);
  pinMode(MB1, OUTPUT);
  pinMode(MB2, OUTPUT);
 
  radio.begin();
  radio.setAutoAck(1);                
  radio.enableAckPayload();         
  radio.setRetries(5,15);
  radio.openReadingPipe(1,address); // for Ard. NANO
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
}

void loop() {
  if(!is_read_data){
    while(isDataAvailable()){
      read_data();
      if(count>32){
        is_read_data=true;
        if(isValidData()){
          start_=true;
          Serial.println("Data read and is valid.");
        }else{
          Serial.println("Data read But is not valid check your connection.");
        }
      }
      count++;
    }
 }
  
  if(start_){
    for(int i=0;i<36;i++){
      if(path[i]==1){
        forward();
        delay(100);
        Stop();
      }

      if(path[i]==2){
        turnRight();
        delay(50);
        forward();
        delay(100);
        Stop();
      }

      if(path[i]==3){
        turnLeft();
        delay(50);
        forward();
        delay(100);
        Stop();
      }

      delay(4000);
      Stop();
    }
    start_=false;
    //end maze
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

bool isDataAvailable(){
  if (radio.available()) {
    Serial.print("Received Data : ");
    return true;
  }
  Serial.println("Not Receiving !!!");
  return false;
}


void read_data(){
  if (j<32){
    radio.read(&data, sizeof(data));
    path[++j]=data;
    Serial.println(path[j]);
    delay(1000);
    }
}

bool isValidData(){
  int count=0;
  for(int i=0;i<32;i++){
    if(path[i] > 0 && (path[i]==1 || path[i]==2 || path[i]==3)){
      continue;
    }
    if(path[i]==0){
      count++;
    }
    delay(200);
 }
 if(count>30){
  return false;
 }
 return true;
}