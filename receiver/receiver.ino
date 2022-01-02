#include "NEET_RF24.h"

NEET_RF24 radio(9, 10, 121);

void setup(){
  Serial.begin(115200);
  Serial.println("[RX] Started receiver");

  radio.begin();
}

void loop(){
  if (radio.rxUpdate()) {
    ControlInput input = radio.rxGetInput();
    if (input.j1PotX % 5 == 0){
      radio.rxSendTelemetry("[RX] Hello this is a long message that will be long " + String(input.j1PotX));
    }
    // Serial.println(input.j1PotX);
  }
  
  delay(40);
}