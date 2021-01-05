#include "nRF_common.h"
#include <Arduino.h>

#include "printf.h"

RF24 radio(9, 10);

uint8_t tx_buf = 0;
byte rx_buf;

void setup() {
    Serial.begin(115200);
    printf_begin();

    radio.begin();
    configure_radio(&radio, 42, true);
    radio.printPrettyDetails();
}

void loop() {
    // put your main code here, to run repeatedly:
    if (radio.write(&tx_buf, sizeof(tx_buf))){
      if (!radio.available()){
        Serial.println("Normal Ack");
      } else {
        while(radio.available()){
          radio.read(&rx_buf, 1);
          printf("Ack Payload: %d", (uint8_t) rx_buf);
        }
      }
    }
    tx_buf++;
    delay(500);
}