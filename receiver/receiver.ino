#include "NEET_RF24.h"

#define NRF_CE 38
#define NRF_CSN 39
#define CHANNEL 25
NEET_RF24 radio(NRF_CE, NRF_CSN, CHANNEL);

void setup(){
  Serial.begin(115200);
  Serial.println("[RX] Started receiver");

  // Flash LED to show that the receiver is running
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);

  // Initialize the receiver and report any errors
  if (!radio.begin()){
    while(1){
      Serial.println("Radio not initialized");
      delay(1000);
    }
  }
}

void loop(){
  // Call rxUpdate to process date, it will return false if the data is not valid (i.e. a lost connection)
  if (radio.rxUpdate()) {

    // Puts the data into input
    ControlInput input = radio.rxGetInput();

    // Access the variables in the input
    if (input.j1PotX > 0){
      // This is how you send data back to the transmitter
      radio.rxSendTelemetry("[RX] Hello " + String(input.j1PotX));
    }
  } else {
    // If the data is not valid, print an error message
    Serial.println("[RX] Invalid data");

    // You should also safely stop the robot when the data is invalid
  }
}