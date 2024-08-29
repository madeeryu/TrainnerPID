//definisi 
const int PIN_MOTOR_A = 7;
const int PIN_MOTOR_B = 8;
const int PIN_MOTOR_ENABLE = 6;
const int PIN_ENCODER_A = 2;
const int PIN_ENCODER_B = 3;
const int PIN_POTENTIO = A0;

int kecepatan;
int Ts = 60;
long int next;

void setup()
{
  Serial.begin(9600);
  
  analogReference(INTERNAL);
    
  pinMode(PIN_MOTOR_A, OUTPUT);
  pinMode(PIN_MOTOR_B, OUTPUT);
  pinMode(PIN_MOTOR_ENABLE, OUTPUT);
  
  pinMode(PIN_ENCODER_A, INPUT_PULLUP);
  attachInterrupt(0, doEncoderA, CHANGE);
  pinMode(PIN_ENCODER_B, INPUT_PULLUP);
  attachInterrupt(1, doEncoderB, CHANGE);
  
  pinMode(PIN_POTENTIO, INPUT);
  
  next = Ts;
}
// pengaturan sudut
long unsigned int encoderPos = 0;
long unsigned int prvEncoderPos = 0;
float posToAngle = 188.0*360/60/4064;
int posInRevolution = 360/posToAngle;
float getCurrentAngle() 
{
  float angle = posToAngle*encoderPos;
  
  if(angle < 0.0) {
    angle += 360;
    encoderPos += posInRevolution;
  }
  else if(angle > 360.0) {
    angle -= 360;
    encoderPos -= posInRevolution;
  }
  
  return angle;
}
// perhitungan RPM
float prvAngle = 0;
int currentRPM = 0;
float getCurrentRPM() 
{
  return currentRPM;
}
//PID declaration
// float kp = 0.01, kd = 0.03, ki = 0.02;
// float kp = 0.001, kd = 0.002, ki = 0.002;
// float kp = 0.1, kd = 0.2, ki = 0.2;
float kp = 0.03, kd = 0.004, ki = 0.04;
//float kp = 1e-4, kd = 1e-4, ki = 4e-4;
//float kp = 3e-4, kd = 1e-4, ki = 5e-4;
//float kp = 0, kd = 0, ki = 5e-4;
//float kp = 0, kd = 0, ki = 1e-4;
// float kp = 7e-4, kd = 9e-4, ki = 9e-4;
//float kp = 0.002, kd = 0.004, ki = 0.004;
//float kp = 2e-3, kd = 4e-3, ki = 4e-3;

int error;
float errorterakhir = 0.0;
float integral = 0.0;

// loopnya
void loop()
{
  int currentTime = millis();
  if(currentTime < next) return;
  next = currentTime + Ts;
  
  //calculate rpm
  float currentAngle = getCurrentAngle();
  float delta = currentAngle - prvAngle;
  
  if(prvAngle > 270 && currentAngle < 90) {
    delta += 360.0;
  }
  else if(prvAngle < 90 && currentAngle > 270) {
    delta -= 360.0;
  }
    
  currentRPM = delta/360.0/Ts*1000*60;
  // currentRPM = getCurrentAngle();
  prvAngle = currentAngle;
  
  float target = analogRead(PIN_POTENTIO);//*1100.0/1028.0;
  int RPM_d = map(target, 0, 1023, 0, 555); //
  //pid program
  error = 350 - currentRPM;
  float derivatif = error - errorterakhir;
  integral += error;
  float out = kp*error + kd*derivatif + ki*integral;
  errorterakhir = error;
  
  if (out > 250){
    out = 250;
    //error = 0;
  }
  float rpm =(out * currentRPM)/230;


  //Serial.print("RPM : ");
  Serial.print(getCurrentRPM());
  Serial.print(" ,");
  Serial.print(" CAngle: ");
  Serial.print(getCurrentAngle());
  Serial.print(" ,");
  Serial.print("Error : ");
  Serial.print(error);
  Serial.print(",");
  Serial.print("RPM :");
  Serial.print(rpm);
  Serial.print(", ");
  Serial.print("CRPM :");
  Serial.print(currentRPM);
  // Serial.print(" POtensio :");
  // Serial.print(target);
  Serial.print(" out :");
  Serial.print(out);
  //Serial.print(rpm);
  Serial.println();
  
  digitalWrite(PIN_MOTOR_A, HIGH);

  analogWrite(PIN_MOTOR_ENABLE, out);
}

// void setSpeed(float val) {
//   // if(val > 1) val = 1;
    
//   // digitalWrite(PIN_MOTOR_A, LOW);
//   // digitalWrite(PIN_MOTOR_B, LOW);
  
//   if(val > 0) {
//     digitalWrite(PIN_MOTOR_B, HIGH);
//     analogWrite(PIN_MOTOR_ENABLE, val);
//   }
//   else {
//     analogWrite(PIN_MOTOR_ENABLE, 0);
//   }
//   Serial.print(val);
// }

void doEncoderA()
{  
  encoderPos += (digitalRead(PIN_ENCODER_A)==digitalRead(PIN_ENCODER_B))?1:-1;
}
void doEncoderB()
{  
  encoderPos += (digitalRead(PIN_ENCODER_A)==digitalRead(PIN_ENCODER_B))?-1:1;
}