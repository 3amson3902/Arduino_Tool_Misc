// Arduino Nano RS485 Reader
#include <SoftwareSerial.h>

// Define the control pin for RS485 direction
#define RS485_CONTROL_PIN 2

// Create a SoftwareSerial object for RS485 communication
// You can use Hardware Serial (Serial) if not needed for other purposes
SoftwareSerial RS485Serial(10, 11); // RX, TX (using software serial for flexibility)

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  
  // Initialize RS485 communication
  RS485Serial.begin(9600); // Match this with your device's baud rate
  
  // Set the RS485 control pin as output
  pinMode(RS485_CONTROL_PIN, OUTPUT);
  
  // Set to receive mode initially
  digitalWrite(RS485_CONTROL_PIN, LOW);
  
  Serial.println("Arduino Nano RS485 Reader Initialized");
}

void loop() {
  // Check if data is available on RS485 bus
  if (RS485Serial.available()) {
    // Read the incoming data
    String receivedData = RS485Serial.readStringUntil('\n'); // Adjust terminator as needed
    
    // Print the received data to serial monitor
    Serial.print("Received: ");
    Serial.println(receivedData);
  }
  
  // Optional: Send a request periodically if your RS485 device needs it
  static unsigned long lastRequestTime = 0;
  if (millis() - lastRequestTime > 5000) { // Every 5 seconds
    sendRS485Request("REQUEST_DATA\n"); // Adjust request format as needed
    lastRequestTime = millis();
  }
}

// Function to send data over RS485
void sendRS485Request(String request) {
  // Enable transmit mode
  digitalWrite(RS485_CONTROL_PIN, HIGH);
  
  // Send the request
  RS485Serial.print(request);
  
  // Wait for transmission to complete
  RS485Serial.flush();
  
  // Return to receive mode
  digitalWrite(RS485_CONTROL_PIN, LOW);
  
  Serial.print("Sent: ");
  Serial.println(request);
}