#include <SoftwareSerial.h>
#define LEDIN 3

SoftwareSerial BT(10, 11); // RX = 10, TX = 11
String btBuffer = "";

void setup() {
  Serial.begin(9600);
  BT.begin(9600);
  Serial.println("System ready. Connect Bluetooth app...");
  BT.println("Arduino READY");
  setPinState(LEDIN,true);
  delay(1000);
}

bool setPinState(int pin_number, bool pin_state) {
  pinMode(pin_number, OUTPUT);
  digitalWrite(pin_number, pin_state);
  return true;
}


void loop() {
  while (BT.available()) {
    char c = BT.read();

    if (c == '\n' || c == '\r') {
      if (btBuffer.length() > 0) {
        btBuffer.trim();
        Serial.print("[BT -> Arduino] ");
        Serial.println(btBuffer);

        if(btBuffer.equalsIgnoreCase("on")){
          setPinState(LEDIN,true);
          BT.println("200 OK: LED_  on");
          Serial.println("200 OK: LED_  on");
        }if(btBuffer.equalsIgnoreCase("off")){
          setPinState(LEDIN,false);
          BT.println("200 OK: LED_  off");
          Serial.println("200 OK: LED_  off");
        }

        btBuffer = "";
      }
    } else {
      btBuffer += c;
    }
  }

  // Allow sending data from Serial Monitor to BT for debugging
  if (Serial.available()) {
    char pcChar = Serial.read();
    if (pcChar != '\r' && pcChar != '\n') {
      Serial.print("[PC -> BT] Sending: ");
      Serial.println(pcChar);
      BT.print(pcChar);
    }
  }
}
