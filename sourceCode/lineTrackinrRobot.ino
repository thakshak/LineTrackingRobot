#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <NewPing.h>

int LQRE1113_Pin = 2; //connected to digital 2
int RQRE1113_Pin = 4; //connected to digital 4

#define commonAnode true
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
long coltime=millis();
bool colorSensorAccessible=true;

//motorShield====================================
// Clockwise and counter-clockwise definitions.
// Depending on how you wired your motors, you may need to swap.
#define CW  0
#define CCW 1
// Motor definitions to make life easier:
#define MOTOR_A 0
#define MOTOR_B 1
// Pin Assignments
// Don't change these! These pins are statically defined by shield layout
const byte PWMA = 3;  // PWM control (speed) for motor A
const byte PWMB = 11; // PWM control (speed) for motor B
const byte DIRA = 12; // Direction control for motor A
const byte DIRB = 13; // Direction control for motor B
int motor_speed;
//========================================

//===ultrasonic======================
#define TRIGGER_PIN  7  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     6  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 30 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
//===================================

int a,b;



void setup() {
  Serial.begin(9600);
  setupArdumoto(); // Set all pins as outputs
  if (tcs.begin()) 
  {
    Serial.println("Found color sensor");
  }
  else
  {
    Serial.println("No TCS34725 found ... check your connections");
    colorSensorAccessible=false;
  }
  pinMode(8, INPUT);
}

void loop()
{
  //===================color sensor==============//
  uint16_t clear, red, green, blue;
  if(colorSensorAccessible)
  {
    tcs.setInterrupt(false);      // turn on LED
    delay(60);  // takes 50ms to read 
    tcs.getRawData(&red, &green, &blue, &clear);
    tcs.setInterrupt(true);  // turn off LED  
    if(red>200 && blue<100 && green<100 && millis()-coltime>3000)
    {
      coltime=millis();
      stopArdumoto(MOTOR_B);
      stopArdumoto(MOTOR_A);
      delay(2000);
    }
  }
  //===================color sensor============//
  //===============ultra sonic sensor=========//
  //delay(50);// Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  //Serial.print("Ping: ");
  int distance_from_obstacle=sonar.ping_cm();
  //Serial.print(distance_from_obstacle); // Send ping, get distance in cm and print result (0 = outside set distance range)
  //Serial.println("cm");
  //Serial.println();
  if(distance_from_obstacle>0 && distance_from_obstacle<30)
  {
    stopArdumoto(MOTOR_B);
    stopArdumoto(MOTOR_A);
    delay(2000);
  }
  
  //===============ultra sonic sensor========//
  //drive 
  int LQRE_Value = readQD(LQRE1113_Pin);
  //Serial.print(LQRE_Value); 
  //Serial.print("  ");
  int RQRE_Value = readQD(RQRE1113_Pin);
  //Serial.println(RQRE_Value);
  //Serial.println();
  motor_speed=200;
  if(digitalRead(8)==LOW) motor_speed = 127;
  while(true)
  {
    if(readQD(LQRE1113_Pin)<3000 && readQD(RQRE1113_Pin)<3000)
    {
      if(a==1) //drive right motor ccw.
      {
        driveArdumoto(MOTOR_B, CCW, 125 ); 
        driveArdumoto(MOTOR_A, CW, 125 );
      }
      else //drive left motor ccw
      {   
        driveArdumoto(MOTOR_B, CW, 125 ); 
        driveArdumoto(MOTOR_A, CCW, 125 );  
      }
    
    }
    else
    {
      stopArdumoto(MOTOR_B);
      stopArdumoto(MOTOR_A);
      break;
    }
  }
  a=0;
  if(LQRE_Value>=3000)
  {
     driveArdumoto(MOTOR_B, CCW, motor_speed );
     a=1;
  }
  else 
  {
    stopArdumoto(MOTOR_B);
    if(b==0)
    a=1;
  }
  b=0;
  if(RQRE_Value>=3000)
  {
    driveArdumoto(MOTOR_A, CCW, motor_speed);
    b=1;
  }
  else 
  {
    stopArdumoto(MOTOR_A);
    if(a==0)
    b=1;
  }
}


int readQD(int sen){
  //Returns value from the QRE1113 
  //Lower numbers mean more refleacive
  //More than 3000 means nothing was reflected.
  pinMode( sen, OUTPUT );
  digitalWrite( sen, HIGH );  
  delayMicroseconds(10);
  pinMode( sen, INPUT );
  long time = micros();
  
  //time how long the input is HIGH, but quit after 3ms as nothing happens after that
  while (digitalRead(sen) == HIGH && micros() - time < 3000); 
  int diff = micros() - time;
  return diff;
}

// driveArdumoto drives 'motor' in 'dir' direction at 'spd' speed
void driveArdumoto(byte motor, byte dir, byte spd)
{
  if (motor == MOTOR_A)
  {
    digitalWrite(DIRA, dir);
    analogWrite(PWMA, spd);
  }
  else if (motor == MOTOR_B)
  {
    digitalWrite(DIRB, dir);
    analogWrite(PWMB, spd);
  }  
}

// stopArdumoto makes a motor stop
void stopArdumoto(byte motor)
{
  driveArdumoto(motor, 0, 0);
}

// setupArdumoto initialize all pins
void setupArdumoto()
{
  // All pins should be setup as outputs:
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(DIRA, OUTPUT);
  pinMode(DIRB, OUTPUT);

  // Initialize all pins as low:
  digitalWrite(PWMA, LOW);
  digitalWrite(PWMB, LOW);
  digitalWrite(DIRA, LOW);
  digitalWrite(DIRB, LOW);
}
