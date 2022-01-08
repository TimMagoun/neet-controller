#include "NEET_RF24.h"

#define NRF_CE 14
#define NRF_CSN 17
NEET_RF24 radio(NRF_CE, NRF_CSN, 25);

void setup(){
  Serial.begin(115200);
  Serial.println("[RX] Started receiver");
  pinMode(6, OUTPUT);
  digitalWrite(6, HIGH);
  delay(200);
  digitalWrite(6, LOW);
  if (!radio.begin()){
    while(1){
      Serial.println("Radio not initialized");
      delay(1000);
    }
  }
}

void loop(){
  if (radio.rxUpdate()) {
    ControlInput input = radio.rxGetInput();
    if (input.j1PotX > 0){
      radio.rxSendTelemetry("[RX] Hello this is a long message that will be long " + String(input.j1PotX));
    }
    // Serial.println(input.j1PotX);
  }
  
  delay(40);
}