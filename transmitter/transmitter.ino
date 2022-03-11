#define CHANNEL 11

#define LED_IND 6
#define JOY_SWITCH 7
#define JOY_X 27
#define JOY_Y 26
#define JOY_BTN_1 22
#define JOY_BTN_2 9
#define VAR_RES 28
#define BTN_1 20
#define BTN_2 21
#define BTN_3 12
#define BTN_4 13
#define TOGGLE_SWITCH 8
#define NRF_CE 14
#define NRF_CSN 17

#include "NEET_RF24.h"
#include "singleLEDLibrary.h"

NEET_RF24 radio(NRF_CE, NRF_CSN, CHANNEL, true);
char buf[MAX_TELEM_STRING_LEN + 1];
ControlInput in;
sllib led_ind(LED_IND, 50);     // LED object for non-blocking blinking
// LED Patterns
int double_flash[] = {100, 100, 100, 700};

void setup(){
  Serial.begin(115200);
  if (radio.begin()){
    Serial.println("[TX] Radio started");
    led_ind.setBreathSingle(1000);
  } else {
    led_ind.setPatternSingle(double_flash, 4);
    while(1){
      if (millis() % 1000 == 0){
        Serial.println("[ERR] Radio not started, check wiring?");
      }
      led_ind.update();
    }
  }
  Serial.flush();
  
  pinMode(JOY_SWITCH, OUTPUT);
  digitalWrite(JOY_SWITCH, LOW);
  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);
  pinMode(VAR_RES, INPUT);
  pinMode(JOY_BTN_1, INPUT_PULLUP);
  pinMode(JOY_BTN_2, INPUT_PULLUP);
  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);
  pinMode(BTN_3, INPUT_PULLUP);
  pinMode(BTN_4, INPUT_PULLUP);
  pinMode(TOGGLE_SWITCH, INPUT_PULLUP);
  
}

void loop(){
  unsigned long last_time = 0;
  unsigned long cur_time = 0;

  // Loop runs at CONTROLLER_RATE_HZ
  while (1){
    // Update menu system
    led_ind.update();

    cur_time = millis();
    if ((cur_time - last_time) > (1000 / CONTROLLER_RATE_HZ)){
      readInputs(in);

      if (radio.txSendControlInput(in)){
        // Serial.println("Transmit success");
        led_ind.setOnSingle();
      } else {
        led_ind.setBlinkSingle(500);
      }

      uint8_t len = radio.txGetTelemetry(buf);      
      last_time = cur_time;
    }
  }
}

void readInputs(ControlInput &in){
  in.j2PotX = map(analogRead(JOY_X), 0, 4095, 127, -127);
  in.j2PotY = map(analogRead(JOY_Y), 0, 4095, -127, 127);
  in.j2Button = !digitalRead(JOY_BTN_2);

  digitalWrite(JOY_SWITCH, HIGH);
  delayMicroseconds(50);
  in.j1PotX = map(analogRead(JOY_X), 0, 4095, 127, -127);
  in.j1PotY = map(analogRead(JOY_Y), 0, 4095, -127, 127);
  digitalWrite(JOY_SWITCH, LOW);

  in.j1Button = !digitalRead(JOY_BTN_1);
  in.pot = map(analogRead(VAR_RES), 0, 4095, -127, 127);
  in.tSwitch = !digitalRead(TOGGLE_SWITCH);
  in.button1 = !digitalRead(BTN_1);
  in.button2 = !digitalRead(BTN_2);
  in.button3 = !digitalRead(BTN_3);
  in.button4 = !digitalRead(BTN_4);
}

void displayInputs(ControlInput &in){
  char buf[128];
  sprintf(buf, "Joy1: % 4d, % 4d | Joy2: % 4d, % 4d | Pot: % 4d | Switch: %d | J1Btn: %d | J2Btn: %d | Btns %d %d %d %d \n"
            , in.j1PotX, in.j1PotY, in.j2PotX, in.j2PotY, in.pot, in.tSwitch, in.j1Button, in.j2Button
            , in.button1, in.button2, in.button3, in.button4);
  Serial.print(buf);
}