#include <nRF_common.h>

bool isTransmit = 0;
/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(9, 10);
byte pipeNo, gotByte;              // Declare variables for the pipe and the byte received
    
void setup() {
    Serial.begin(115200);
    // Setup and configure radio
    configure_radio(radio, 42, isTransmit);
}

void loop(void) {
    while (radio.available(&pipeNo)) { // Read all available payloads
        radio.read(&gotByte, 1);
        // Since this is a call-response. Respond directly with an ack payload. ss
        radio.writeAckPayload(pipeNo, &gotByte, 1); // This can be commented out to send empty payloads.
        Serial.print(F("Loaded next response "));
        Serial.println(gotByte);
    }
}