# neetam-controller
Functional nRF24L01 based RC transmitter and receiver, running on a Pi Pico RP2040. This currently supports telemetry with ack payloads and configurable settings on the transmitter using ArduinoMenu.

## Requirements
- RF24 library
- ArduinoMenu library

Both can be installed via the Arduino IDE library manager

## Getting Started with the Receiver

There are a few methods that you will use to receive information from the transmitter. You can look at `receiver/receiver.ino`, which is a bare minimum sketch for reference.

### Connecting the radio module 
The radio should be connected to the Arduino via an SPI interface, which consists of `CLK` (clock), `COPI` (controller out, peripheral in), `CIPO` (controller in, peripheral out), and `CSN` (chip select). There is an additional pin `CE` (chip enable) which also needs to be connected to a digital output pin. If you are using a sensor shield, these pins are already wired to predefined pins, otherwise `CE` and `CSN` can be connected to any digital output pins while the other pins should be connected to the hardware SPI pins on the Arduino. 

### Including the file

Like other libraries, we will use an `#include` directive to link the controller library to our code. Under the hood, the compiler will copy the content of the included file into your current file, which will then be compiled. Be careful not to redefine any of the macros in the included file, such as `CONTROLLER_RATE_HZ` or `RECEIVER_TIMEOUT_MSEC`, because it will change the behavior of the prewritten code.

Add `#include "NEET_RF24.h"` to the top of your file, make sure that `NEET_RF24.h` and `NEET_RF24.cpp` is in the same folder as your `.ino` file so that the Arduino IDE can find it.

### Declaring and initializing the radio object

On top of your sketch, declare the radio object by writing these four lines:

```c++
#define NRF_CE 38
#define NRF_CSN 39
#define CHANNEL 25
NEET_RF24 radio(NRF_CE, NRF_CSN, CHANNEL);
```

Be sure to change the `NRF_CE`, `NRF_CSN`, and `CHANNEL` to the values you are using.

The `begin()` function should be called once in `setup()` to initialize the radio object. It will return true if the radio is sucessfully initialized.

### Getting values from the transmitter

You should periodically call `rxUpdate()` to let the radio module process incoming data. The transmitter is configured to send at 40Hz so calling this function at that rate will provide the most up-to-date values from the controller. To detect disconnections from the transmitter, `rxUpdate()` will return if data is received from the transmitter in the last 100ms. You can use that return value to determine if it is safe to continue to drive/operate the robot or if it is better to halt and wait.

After calling `rxUpdate()` you can get the values from the transmitter by calling `rxGetInput()`. It will return a `ControlInput` struct that contains the values of the transmitter.

A recommended way to read and process the data is as follows: 
```c++
ControlInput input_vals; // Holds value from transmitter

void loop(){
    if (radio.rxUpdate()){
        input_vals = radio.rxGetInput();
        // Do things with the input
    }
    // Do other things...
}
```

### Sending telemetry back

You can also send data back to the transmitter to be displayed via Serial by using the `rxSendTelemetry()` function. The first argument will be a string containing the data you are sending, make sure the length of the message is below 63 characters to keep the telemetry brief and easy to end, the second argument is optional and indicates what to do if the radio is in the middle of transmitting the previous telemetry message. Setting it to true will interrupt the previous transmission, while setting it to false will discard the new telemetry. An example is below:

```c++

// Sends "Hello world!" and the content of x
radio.rxSendTelemetry("Hello world! " + String(x));

// Sends a message with override enabled
radio.rxSendTelemetry("This is urgent", true);
```

## Getting Started with the Transmitter

The transmitter has two joy sticks, six buttons, one potentiometer, and a toggle switch. It is pre-loaded with firmware that allows it to transmit the values of those inputs at 40Hz, as well as receive and display telemetry from a paired receiver. There is a menu system that allows you to configure the controller's channel and display options, and it's accessed via the Serial monitor. You don't need to upload code to the controller, if you accidentally overwrite the code on the controller please let a course staff know so we can upload the correct code. 

### Diagram of the controller

### Changing the Channel

### Changing display options



## WIP

Features that are in progress
    - send custom instructions such as start autonomous
    - send data field for on-the-fly parameter tuning (e.g. PID)