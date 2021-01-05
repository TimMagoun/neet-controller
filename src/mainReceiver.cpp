#include <Arduino.h>

#include "nRF_common.h"
#include "printf.h"
RF24 radio(9, 10);

byte pipe_num, rx_buf;

void setup() {
    Serial.begin(115200);
    printf_begin();

    printf("Hello %d world %s\n", 5, "test");
    radio.begin();
    configure_radio(&radio, 42, false);
    radio.writeAckPayload(1, &rx_buf, 1);
    radio.printPrettyDetails();
}

void loop() {
    // put your main code here, to run repeatedly:
    while (radio.available(&pipe_num)) {
        radio.read(&rx_buf, 1);
        if (((uint8_t)rx_buf) % 3 == 0) {
            if (!radio.writeAckPayload(1, &rx_buf, 1)) {
                Serial.println("Acked with payload");
            } else {
              Serial.println("Failed to Ack with payload");
            }
        }
        Serial.println(rx_buf);
    }

    if (radio.failureDetected) {
        Serial.println("Failure Detected");
    }
    delay(1);
}