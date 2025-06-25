#include <SoftwareSerial.h>

// RS485 Configuration
#define RS485_CONTROL_PIN 2  // RE/DE control pin (connect to both RE and DE)
#define RS485_RX_PIN 7       // RO (Receiver Output)
#define RS485_TX_PIN 8       // DI (Driver Input)
#define BAUD_RATE 9600       // MODBUS RTU standard baud rate
#define INTER_FRAME_DELAY 4  // 3.5 character delay in ms (MODBUS spec)

SoftwareSerial RS485Serial(RS485_RX_PIN, RS485_TX_PIN);

// MODBUS RTU frame buffer
byte modbusFrame[256];
byte frameIndex = 0;
unsigned long lastByteTime = 0;

void setup() {
  Serial.begin(115200);  // Higher speed for debugging
  RS485Serial.begin(BAUD_RATE);
  
  pinMode(RS485_CONTROL_PIN, OUTPUT);
  digitalWrite(RS485_CONTROL_PIN, LOW);  // Set to receive mode
  
  Serial.println("RS485 MODBUS RTU Monitor Ready");
  Serial.println("-----------------------------");
}

void loop() {
  receiveFrame();
  sendTestQuery();  // Optional periodic test query
}

void receiveFrame() {
  // Check for incoming data
  while (RS485Serial.available()) {
    byte inByte = RS485Serial.read();
    unsigned long currentTime = millis();
    
    // Check if new frame should start
    if (currentTime - lastByteTime > INTER_FRAME_DELAY && frameIndex > 0) {
      processFrame();
      frameIndex = 0;
    }
    
    // Store byte if buffer not full
    if (frameIndex < 256) {
      modbusFrame[frameIndex++] = inByte;
      lastByteTime = currentTime;
    } else {
      Serial.println("!Buffer overflow");
      frameIndex = 0;
    }
  }
  
  // Check if frame is complete by timeout
  if (frameIndex > 0 && millis() - lastByteTime > INTER_FRAME_DELAY) {
    processFrame();
    frameIndex = 0;
  }
}

void processFrame() {
  Serial.print("RTU Frame [");
  Serial.print(frameIndex);
  Serial.print("]: ");
  
  // Print hex values
  for (byte i = 0; i < frameIndex; i++) {
    if (modbusFrame[i] < 0x10) Serial.print('0');
    Serial.print(modbusFrame[i], HEX);
    Serial.print(' ');
  }
  
  // Print ASCII representation
  Serial.print(" | ");
  for (byte i = 0; i < frameIndex; i++) {
    if (modbusFrame[i] >= 32 && modbusFrame[i] <= 126) {
      Serial.write(modbusFrame[i]);
    } else {
      Serial.print('.');
    }
  }
  Serial.println();
}

// Optional: Send test MODBUS query (Read Holding Registers)
void sendTestQuery() {
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 5000) {  // Send every 5 seconds
    const byte query[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B};
    
    digitalWrite(RS485_CONTROL_PIN, HIGH);  // Enable transmit
    RS485Serial.write(query, sizeof(query));
    RS485Serial.flush();
    digitalWrite(RS485_CONTROL_PIN, LOW);   // Return to receive
    
    Serial.println("Sent MODBUS query: 01 03 00 00 00 02 C4 0B");
    lastSend = millis();
  }
}