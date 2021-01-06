#ifndef NRF_COMMON_H
#define NRF_COMMON_H

#include <Arduino.h> // Needed to access byte datatype to store buttons states
#include <SPI.h>
#include "RF24.h"

const byte addr[][6] = {"rx2tx", "tx2rx"}; 
typedef enum { role_ping_out = 1, role_pong_back } role_e;

typedef struct {
    int8_t joy_x;
    int8_t joy_y;
    byte buttons;
} Transmit_data;


// Configures it with common channel and pipe
int configure_radio(RF24 &p_radio, uint8_t p_channel_num, boolean is_transmit);

#endif