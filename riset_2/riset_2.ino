#include <util/atomic.h> // For the ATOMIC_BLOCK macro
//motor directory
#define CW  0
#define CCW 1
  
//motor control pin
#define motorDirPin 7
#define motorPWMPin 9
#define enablePin 8
  
//encoder pin
#define encoderPinA 2
#define encoderPinB 4
  
//encoder var
volatile int encPos = 0; //variable fungsi interrupt
int encPosRead = 0;
int prevEncPosRead;
 
//variable pengukur kecepatan
int deltaPos;
float targetSpeed;
float speedMotor;
  
//PID control
float Kp          = 0.3;
float Ki          = 10;
float Kd          = 0;
 
//variable rumus PID
float   error;
float   errortmin1;
float   integral;
float   derivative;
float   dt = 0.060; //second
int     dtMillis = dt*1000; //milisecond
float   control;
long int currTime;
long int prevTime;
long int delTime;
 
//variable pengaturan PWM motor
float velocity;
int   PWMvelocity;
int   dirMotor = 0;
 
//mode program
char modeProg;
  
//fungsi external interrupt encoder
void doEncoderA()
{
  digitalRead(encoderPinB)?encPos++:encPos--;
}
 
void setup()
{
  //setup interrupt
    pinMode(encoderPinA, INPUT_PULLUP);
    pinMode(encoderPinB, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(encoderPinA), doEncoderA,RISING);
    
    //setup motor driver
    pinMode(motorDirPin, OUTPUT);
    pinMode(enablePin, OUTPUT);
    digitalWrite(enablePin, HIGH);
    
    Serial.begin(9600); //setting serial
    analogReadResolution(8); //setting adc 8bit
    digitalWrite(motorDirPin, CW); //set arah motor default
 
    modeProg = 0; //pilih mode cek speed 1, run 0
    //========Mode cek speed==============
    if(modeProg)
    { 
      for(int j=0;j<=255;j+=5)
      {
        PWMvelocity = j; //test perbandingan speed dan PWM
        prevTime = millis();       
        analogWrite(motorPWMPin, PWMvelocity);        
           
        do
        { 
          currTime = millis();
          delTime = currTime-prevTime;
        }while(delTime<dtMillis);
        
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {//fungsi membaca variable dari interrupt
          encPosRead = encPos;
          encPos = 0;    
        }
        deltaPos = encPosRead - prevEncPosRead;
        prevEncPosRead = 0;
         
        //encoder pada motor ini, satu putaran 360 derajat didapatkan 100 step
        //kecepatan motor dihitung dalam 60 ms, satuan rad/s
        speedMotor = (float)deltaPos/dt/100;//menghitung kecepatan 
        digitalWrite(motorDirPin, CW);
        analogWrite(motorPWMPin, 0);  
         
        //Serial.print("raw_speed:");
        Serial.println(speedMotor);
         
        //pengendalian agar nilai dt sama tiap cycle
        //tidak menggunakana delay       
        do
        {
          currTime = millis();
          delTime = currTime-prevTime;
        }while(delTime<1500);
      }     
      while(1);
    }
}
 
void loop()
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {//fungsi membaca variable dari interrupt
    encPosRead = encPos;    
  }
  deltaPos = encPosRead - prevEncPosRead;
  prevEncPosRead = encPosRead;
   
  //encoder pada motor ini, satu putaran 360 derajat didapatkan 100 step
  //jadi kecepatan motor dihitung dalam rotasi/detik
  speedMotor = (float)deltaPos/dt/100;//menghitung kecepatan  
 
  //max adc 255, nilai tengah 0, set kecepatan dari -125 CCW samapi 125 CW  
  targetSpeed = ((float)analogRead(A0)-125)/10;//dijadikan skala -12.5 sampai 12.5   
 
  //PID
  error   = targetSpeed - speedMotor;
  integral += dt*error; 
  derivative = (error - errortmin1)/dt;
  control = (Kp*error) + (Ki*integral) + (Kd*derivative);
   
  //pengaturan PWM
  //konversi dari kecepatan rad/sec ke PWM berdasarkan mode cek speed
  velocity = (control*14.891)+12.135;
  PWMvelocity = min(max(velocity, -255), 255);
  if(PWMvelocity >= 0)
  {
      digitalWrite(motorDirPin, CW);
      analogWrite(motorPWMPin, PWMvelocity); 
  }
  else
  {
      digitalWrite(motorDirPin, CCW);
      analogWrite(motorPWMPin, 255+PWMvelocity);
  }  
 
  //===========Serial: ploter
  Serial.print("nol:");
  Serial.print(0);//titik tengah 0
  Serial.print(" ");
  Serial.print("target_speed:");
  Serial.print(targetSpeed);
  Serial.print(" ");
  Serial.print("speedMotor:");
  Serial.println(speedMotor); //plot data  
 
  //pengendalian agar nilai dt sama tiap cycle
  //tidak menggunakana delay
  //berdasarkan pengukuran, setiap serial print memakan waktu 5ms
  //anggap saja kita mengambil dt aman di 60ms
  do
  {
    currTime = millis();
    delTime = currTime-prevTime;
  }while(delTime<dtMillis);
 
  prevTime = currTime;
  errortmin1 = error;
}