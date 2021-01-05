#include "nRF_common.h"

const byte tx_to_rx_pipe[] = "tx2rx";
const byte rx_to_tx_pipe[] = "rx2tx";
const rf24_pa_dbm_e pa_level = RF24_PA_LOW;

/**
 * Configures the radio with a common channel, pipe addresses, and various transmission settings
 * 
 * Returns 0 if successful
 */

int configure_radio(const RF24 *pRadio, uint8_t p_channel_num, boolean isTransmit) {
    if (p_channel_num > 125) {
        return 1;
    }

    RF24 r = *pRadio;
    r.setPALevel(pa_level);
    r.setChannel(p_channel_num);
    r.setRetries(5, 5);
    r.enableAckPayload();
    r.enableDynamicPayloads();

    if (isTransmit) {
        r.openWritingPipe(tx_to_rx_pipe);
        r.openReadingPipe(1, rx_to_tx_pipe);
        r.stopListening();
    } else {
        r.openWritingPipe(rx_to_tx_pipe);
        r.openReadingPipe(1, tx_to_rx_pipe);
        r.startListening();
    }

    return 0;
}