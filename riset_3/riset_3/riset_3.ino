//************ DEFINITIONS************
float kp = 0.02;
float ki = 0.00015;
float kd = 0.001;

unsigned long t;
unsigned long t_prev = 0;

const byte interruptPinA = 2;
const byte interruptPinB = 3;
volatile long EncoderCount = 0;
const byte PWMPin = 6;
const byte DirPin1 = 7;
const byte DirPin2 = 8;

volatile unsigned long count = 0;
unsigned long count_prev = 0;

float Theta, RPM, RPM_d;
float Theta_prev = 0;
int dt;
const int PotPin = A0;  // Pin analog untuk potensiometer

#define pi 3.1416
float Vmax = 10;
float Vmin = 0;  // Tidak ada nilai negatif karena hanya satu arah
float V = 0.1;
float e, e_prev = 0, inte, inte_prev = 0;

//**********FUNCTIONS******************
//     Void ISR_EncoderA
//     Void ISR_EncoderB
//     Void Motor Driver Write
//     Timer Interrupt
//*************************************
void ISR_EncoderA() {
  bool PinB = digitalRead(interruptPinB);
  bool PinA = digitalRead(interruptPinA);

  if (PinB == LOW) {
    if (PinA == HIGH) {
      EncoderCount++;
    }
    else {
      EncoderCount--;
    }
  }
  else {
    if (PinA == HIGH) {
      EncoderCount--;
    }
    else {
      EncoderCount++;
    }
  }
}

void ISR_EncoderB() {
  bool PinB = digitalRead(interruptPinA);
  bool PinA = digitalRead(interruptPinB);

  if (PinA == LOW) {
    if (PinB == HIGH) {
      EncoderCount--;
    }
    else {
      EncoderCount++;
    }
  }
  else {
    if (PinB == HIGH) {
      EncoderCount++;
    }
    else {
      EncoderCount--;
    }
  }
}

float sign(float x) {
  if (x > 0) {
    return 1;
  } else if (x < 0) {
    return -1;
  } else {
    return 0;
  }
}

//***Motor Driver Functions*****

void WriteDriverVoltage(float V, float Vmax) {
  int PWMval = int(555 * abs(V) / Vmax);
  if (PWMval > 555) {
    PWMval = 555;
  }
  // Motor hanya bergerak maju
  digitalWrite(DirPin1, HIGH);
  digitalWrite(DirPin2, LOW);
  analogWrite(PWMPin, PWMval);
}

void setup() {
  Serial.begin(9600);
  pinMode(interruptPinA, INPUT_PULLUP);
  pinMode(interruptPinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPinA), ISR_EncoderA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(interruptPinB), ISR_EncoderB, CHANGE);
  pinMode(DirPin1, OUTPUT);
  pinMode(DirPin2, OUTPUT);

  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 12499; //Prescaler = 64
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS11 | 1 << CS10);
  TIMSK1 |= (1 << OCIE1A);
  sei();
}

void loop() {
  // Membaca nilai potensiometer dan mengonversinya menjadi kecepatan target
  int potValue = analogRead(PotPin);
  RPM_d = map(potValue, 0, 1023, 0, 500); // Rentang kecepatan 0 - 500 RPM

  if (count > count_prev) {
    t = millis();
    Theta = EncoderCount / 900.0;
    dt = (t - t_prev);
    
    RPM = (Theta - Theta_prev) / (dt / 1000.0) * 60;
    e = RPM_d - RPM;
    inte = inte_prev + (dt * (e + e_prev) / 2);
    V = kp * e + ki * inte + (kd * (e - e_prev) / dt);
    if (V > Vmax) {
      V = Vmax;
      inte = inte_prev;
    }
    if (V < Vmin) {
      V = Vmin;
      inte = inte_prev;
    }

    WriteDriverVoltage(V, Vmax);

    Serial.print("Set RPM : "); Serial.print(RPM_d); Serial.print("\t");
    Serial.print("RPM SENSOR : "); Serial.print(RPM); Serial.print(" \t ");
    Serial.print("PDI : "); Serial.print(V); Serial.print("\t  ");
    Serial.print("Error : "); Serial.println(e);

    Theta_prev = Theta;
    count_prev = count;
    t_prev = t;
    inte_prev = inte;
    e_prev = e;
  }
}

ISR(TIMER1_COMPA_vect) {
  count++;
  Serial.print(count * 0.05); Serial.print(" \t");
}
