#include <SoftwareSerial.h>

SoftwareSerial BT(10, 11); // RX = 10, TX = 11
String btBuffer = "";

void setup() {
  Serial.begin(9600);
  BT.begin(9600);
  Serial.println("System ready. Connect Bluetooth app...");
  BT.println("Arduino READY");
}

bool setPinState(int pin_number, bool pin_state) {
  pinMode(pin_number, OUTPUT);
  digitalWrite(pin_number, pin_state);
  return true;
}

int split(String input, String output[], int maxParts) {
  input.trim();
  int count = 0;
  int start = 0;

  while (count < maxParts) {
    int spaceIndex = input.indexOf(' ', start);

    if (spaceIndex == -1) {
      String token = input.substring(start);
      token.trim();
      if (token.length() > 0) output[count++] = token;
      break;
    }

    String token = input.substring(start, spaceIndex);
    token.trim();
    if (token.length() > 0) output[count++] = token;

    start = spaceIndex + 1;
  }

  return count;
}


void command(String arr[], int len) {
  Serial.println("Command received:");
  for (int i = 0; i < len; i++) {
    Serial.print("Arg[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(arr[i]);
  }

  if (len >= 3 && arr[0] == "set") {
    int pin = arr[1].toInt();
    String state = arr[2];
    state.toLowerCase();

    if (state == "on") {
      if (setPinState(pin, true)) {
        BT.println("200 OK: Pin set HIGH");
        Serial.println("200 OK: Pin set HIGH");
      } else {
        BT.println("500 Error: Failed to set pin");
        Serial.println("500 Error: Failed to set pin");
      }
    } else if (state == "off") {
      if (setPinState(pin, false)) {
        BT.println("200 OK: Pin set LOW");
        Serial.println("200 OK: Pin set LOW");
      } else {
        BT.println("500 Error: Failed to set pin");
        Serial.println("500 Error: Failed to set pin");
      }
    } else {
      BT.println("400 Bad Request: Use 'on' or 'off'");
      Serial.println("400 Bad Request: Use 'on' or 'off'");
    }
  } else {
    BT.println("400 Bad Request: Format - cmd set <pin> <on/off>");
    Serial.println("400 Bad Request: Format - cmd set <pin> <on/off>");
  }
}

void parseAndExecuteCommand(String fullCmd) {
  fullCmd.trim();

  // Properly check for "cmd " prefix
  if (fullCmd.startsWith("cmd ")) {
    fullCmd = fullCmd.substring(4); // Remove "cmd " (note the space)
  } else if (fullCmd.startsWith("cmd")) {
    fullCmd = fullCmd.substring(3); // Fallback in case space is missing
  }

  fullCmd.trim(); // Important!

  const int maxArgs = 10;
  String args[maxArgs];
  int argCount = split(fullCmd, args, maxArgs);

  if (argCount > 0) {
    command(args, argCount);
  } else {
    BT.println("400 Bad Request: Could not parse command");
    Serial.println("400 Bad Request: Could not parse command");
  }
}


void loop() {
  while (BT.available()) {
    char c = BT.read();

    if (c == '\n' || c == '\r') {
      if (btBuffer.length() > 0) {
        btBuffer.trim();
        Serial.print("[BT -> Arduino] ");
        Serial.println(btBuffer);

        if (btBuffer.startsWith("cmd")) {
          parseAndExecuteCommand(btBuffer);
        } else {
          BT.println("200 OK: Message received");
          Serial.println("200 OK: Message received");
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
