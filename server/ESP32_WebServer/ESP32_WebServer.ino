#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "Router";
const char* password = "9876543210";

// Create a web server on port 80
WebServer server(80);

int motorPin = 5;  // Pin for controlling motor speed (PWM)
int motorSpeed = 0;  // Initial motor speed

void handleSetSpeed() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");

    // Parse the JSON body
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, body);
    
    if (error) {
      server.send(400, "application/json", "{\"message\":\"Invalid JSON\"}");
      return;
    }

    // Get the motor speed from the JSON
    motorSpeed = doc["speed"];
    motorSpeed = constrain(motorSpeed, 0, 255);  // Constrain to valid PWM range

    // Set motor speed (PWM)
    analogWrite(motorPin, motorSpeed);

    // Send success response
    server.send(200, "application/json", "{\"message\":\"Speed updated successfully\"}");
  }
}

void setup() {
  // Initialize serial and motor control pin
  Serial.begin(115200);
  pinMode(motorPin, OUTPUT);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set up the web server route for setting speed
  server.on("/set_speed", HTTP_POST, handleSetSpeed);

  // Start the web server
  server.begin();
}

void loop() {
  // Handle incoming client requests
  server.handleClient();
  Serial.println(motorSpeed);
}