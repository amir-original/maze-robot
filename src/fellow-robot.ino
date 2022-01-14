
/**
 *  Fellow Robot
 *  Cooperation of Multiple Robots to Solve Maze Tasks
 *  The circuit:
 *    1.Arduino Uno
 *    3.Dc Motor 100RPM
 *    2.L9110S
 *    3.Ultrasonic HC-SR04
 *    4.Nrf24l01
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

uint32_t path[36];
bool start_=false;
bool is_read_data=false;
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
  radio.openReadingPipe(1,address); // for Ard. NANO
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
}

void loop() {
  if(!is_read_data){
    while(isDataAvailable()){
      get_data();
      if(isValidData()){
        start_=true;
        is_read_data=true;
      }
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

void get_data(){
  radio.read(path, sizeof(path));
  // check 
}

bool isValidData(){
  int count=0;
  for(int i=0;i<36;i++){
    if(path[i] > 0 && (path[i]==1 || path[i]==2 || path[i]==3)){
      return true;
    }
  if(path[i]==0){
    count++;
  }
 }
 if(count>30){
  return false;
 }
}