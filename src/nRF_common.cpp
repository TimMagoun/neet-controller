#include "nRF_common.h"


const rf24_pa_dbm_e pa_level = RF24_PA_LOW;

/**
 * Configures the radio with a common channel, pipe addresses, and various transmission settings
 * 
 * Returns 0 if successful
 */

int configure_radio(RF24 &pRadio, uint8_t p_channel_num, boolean isTransmit) {
    if (p_channel_num > 125){
        return 1;
    }
    
    pRadio.begin();

    pRadio.setPALevel(pa_level);
    pRadio.setRetries(0, 2);
    pRadio.setChannel(p_channel_num);
    pRadio.enableAckPayload();      // Allow optional ack payloads
    pRadio.enableDynamicPayloads(); // Ack payloads are dynamic payloads

    if (isTransmit) {
        pRadio.openWritingPipe(addr[1]);
        pRadio.openReadingPipe(1, addr[0]);
        pRadio.stopListening();
    } else {
        pRadio.openWritingPipe(addr[0]);
        pRadio.openReadingPipe(1, addr[1]);
        pRadio.startListening(); // Start listening
    }

    return 0;
}