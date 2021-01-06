
#include "nRF_common.h"

bool isTransmit = 1;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(9, 10);

byte counter = 1; // A single byte to keep track of the data being sent back and forth

void setup() {
    Serial.begin(115200);
    configure_radio(radio, 42, isTransmit);
}

void loop(void) {

    byte gotByte; // Initialize a variable for the incoming response

    if (counter == 1) {
        Serial.print(F("Now sending ")); // Use a simple byte counter as payload
        Serial.println(counter);
    }

    // unsigned long time = micros(); // Record the current microsecond count

    if (radio.write(&counter, 1)) { // Send the counter variable to the other radio
        if (!radio.available()) {   // If nothing in the buffer, we got an ack but it is blank
            // Serial.print(F("Got blank response. round-trip delay: "));
            // Serial.print(micros() - time);
            // Serial.println(F(" microseconds"));
        } else {
            while (radio.available()) {  // If an ack with payload was received
                radio.read(&gotByte, 1); // Read it, and display the response time
                unsigned long timer = micros();

                // Serial.print(F("Got response "));
                // Serial.print(gotByte);
                // Serial.print(F(" round-trip delay: "));
                // Serial.print(timer - time);
                // Serial.println(F(" microseconds"));
                counter++; // Increment the counter variable
            }
        }

    } else {
        Serial.println(F("Sending failed."));
    } // If no ack response, sending failed

    delay(50);
}