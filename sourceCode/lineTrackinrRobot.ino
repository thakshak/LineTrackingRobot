//QRE1113 Digital board
//Outputs via the serial terminal - Lower numbers mean more reflected
//3000 or more means nothing was reflected.
//line sensor============================
int LQRE1113_Pin = 2; //connected to digital 2
int RQRE1113_Pin = 4; //connected to digital 4
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
//========================================

int a,b;
void setup(){
  Serial.begin(9600);
  setupArdumoto(); // Set all pins as outputs
}


void loop(){

  int LQRE_Value = readQD(LQRE1113_Pin);
  Serial.print(LQRE_Value); 
  Serial.print("  ");
  int RQRE_Value = readQD(RQRE1113_Pin);
  Serial.println(RQRE_Value);
  Serial.println();
  while(true)
  {
  if(readQD(LQRE1113_Pin)<3000 && readQD(RQRE1113_Pin)<3000)
  {
    if(a==1) //drive right motor ccw.
    {
     driveArdumoto(MOTOR_B, CCW, 125 ); 
    driveArdumoto(MOTOR_A, CW, 125 );
    }
    else //drive left ccw
    {
         
    driveArdumoto(MOTOR_B, CW, 125 ); 
    driveArdumoto(MOTOR_A, CCW, 125 );  
    }
 
  }
  else{
    driveArdumoto(MOTOR_B, CCW, 0 );
    driveArdumoto(MOTOR_A, CCW, 0 );
    break;
  }
  }
    a=0;
  if(LQRE_Value>=3000)
  {
     driveArdumoto(MOTOR_B, CCW, 125 );
     a=1;
  }
  else 
  {
    driveArdumoto(MOTOR_B, CCW, 0 );
    if(b==0)
    a=1;
  }
  b=0;
  if(RQRE_Value>=3000)
  {
    driveArdumoto(MOTOR_A, CCW, 125);
    b=1;
  }
  else 
  {
    driveArdumoto(MOTOR_A, CCW, 0);
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
