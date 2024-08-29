// #include <atomic.h> // For the ATOMIC_BLOCK macro

// Motor control pin
#define motorDirPin 7
#define motorPWMPin 9
#define enablePin 8
// Define motor direction constants
#define CW  0
#define CCW 1

float Kp = 0.3; 
float Ki = 10;
float Kd = 0;

float error, previous_error = 0;
float integral = 0;
float derivative;
float dt = 0.060; // seconds
int dtMillis = dt * 1000; // milliseconds
float control;

int PWMvelocity;
int motorDirection = CW;

void setup() {
  pinMode(motorDirPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, HIGH);

  Serial.begin(9600); // Initialize serial communication
  // analogReadResolution(8); // Set ADC resolution to 8 bits
  digitalWrite(motorDirPin, CW); // Set default motor direction
}

void loop() {
  int targetSpeed = map(analogRead(A0), 0, 1023, 0, 255);

  error = targetSpeed ; 
  integral += error * dt;
  derivative = (error - previous_error) / dt;
  control = Kp * error + Ki * integral + Kd * derivative;


  digitalWrite(motorDirPin, CW);
  analogWrite(motorPWMPin, PWMvelocity);

  if (PWMvelocity >= 0) {
  } else {
    digitalWrite(motorDirPin, CCW);
    analogWrite(motorPWMPin, 255 + PWMvelocity); // Reverse speed control
  }

  Serial.print("Target Speed: ");
  Serial.print(targetSpeed);
  Serial.print(" Current PWM: ");
  Serial.println(PWMvelocity);

  // Maintain control frequency
  long currentTime = millis();
  while (millis() - currentTime < dtMillis); // Simple timing control

  previous_error = error; // Update previous error
}


