#define DISP_TELEMETRY 0
#define DISP_CONTROLLER_INPUT 1
#define DISP_NONE 2
#define EEPROM_START_ADDR 0 // Only change if we need to write to a different eeprom block, lifespan 100k writes

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
#include <EEPROM.h>
#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>
#include "singleLEDLibrary.h"

NEET_RF24 radio(NRF_CE, NRF_CSN, 11, true);
char buf[MAX_TELEM_STRING_LEN + 1];
ControlInput in;
sllib led_ind(LED_IND, 50);     // LED object for non-blocking blinking
// LED Patterns
int double_flash[] = {100, 100, 100, 700};

result saveSetting(eventMask e, prompt &item); // Need forward declaration

// Struct to save the settings
struct Setting {
  uint8_t dispOpt;
  uint8_t channel;
};

Setting s;
// Radio enabled menu items, controls whether the radio transmits
bool radio_enabled = true;
TOGGLE(radio_enabled, setRadio, "Radio: ", doNothing, noEvent, noStyle
  ,VALUE("On", true, doNothing, noEvent)
  ,VALUE("Off", false, doNothing, noEvent)
);

// Display option menu items, controls what is displayed through Serial
CHOOSE(s.dispOpt, setDispOpt, "Display: ", saveSetting, exitEvent, noStyle
  ,VALUE("Telemetry", DISP_TELEMETRY, doNothing, noEvent)
  ,VALUE("Controller Input", DISP_CONTROLLER_INPUT, doNothing, noEvent)
  ,VALUE("Nothing", DISP_NONE, doNothing, noEvent)
);

const char* constMEM decDigit MEMMODE="0123456789";
const char* constMEM decNr[] MEMMODE={decDigit, decDigit};
char channel_buf[] = "11";

bool in_menu = false; // Tracks if menu is currently active (not idling)
// Main menu
MENU(mainMenu, "Main menu", doNothing, noEvent, wrapStyle
  ,SUBMENU(setRadio)
  ,SUBMENU(setDispOpt)
  ,EDIT("Channel:", channel_buf, decNr, saveSetting, exitEvent, noStyle)  
  ,EXIT("<< Exit"))

#define MAX_DEPTH 2

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

serialIn serial(Serial);
NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);

// Save result to EEPROM
result saveSetting(eventMask e, prompt &item){
  Serial.println("\n[Saving settings to EEPROM]");
  s.channel = (uint8_t) atoi(channel_buf);  
  radio.txSetChannel(s.channel);
  Serial.println(s.channel);
  Serial.println(s.dispOpt);
  writeSettings(s);
  return proceed;
}

result idle(menuOut &o, idleEvent e) {
  switch(e) {
    case idleStart:
      Serial.println("\nExiting menu, press * to enter");
      in_menu = false;
      break;
    // case idling:o.println("suspended...");break;
    case idleEnd:
      Serial.println("Entering menu");
      in_menu = true;
      nav.reset();
      break;
  }
  return proceed;
}

void setup(){
  Serial.begin(115200);
  // while(!Serial);
  Serial.println("[TX] Started transmitter");
  Serial.println("[TX] Use numbers or +- keys to navigate");
  Serial.println("[TX] Use * to enter menu and select");
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
  
  // Read settings from EEPROM
  EEPROM.begin(256);
  readSettings(s);
  // Configure menu params
  nav.timeOut = 10;
  nav.idleTask = idle;
  nav.idleOn(); // Enter into idle immediately
}

void loop(){
  unsigned long last_time = 0;
  unsigned long cur_time = 0;

  // Loop runs at CONTROLLER_RATE_HZ
  while (1){
    // Update menu system
    nav.poll();
    led_ind.update();

    cur_time = millis();
    if (radio_enabled && (cur_time - last_time) > (1000 / CONTROLLER_RATE_HZ)){
      readInputs(in);

      if (radio.txSendControlInput(in)){
        // Serial.println("Transmit success");
        led_ind.setOnSingle();
      } else {
        led_ind.setBlinkSingle(500);
      }

      uint8_t len = radio.txGetTelemetry(buf);
      // Displaying telemetry or controller inputs
      if (!in_menu){
        if (s.dispOpt == DISP_TELEMETRY && len != 0){
          Serial.println(buf);
        } else if (s.dispOpt == DISP_CONTROLLER_INPUT){
          displayInputs(in);
        }
      }
      
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

void readSettings(Setting &s){
  EEPROM.get(EEPROM_START_ADDR, s);
  channel_buf[0] = '0' + s.channel / 10;
  channel_buf[1] = '0' + s.channel % 10;
  radio.txSetChannel(s.channel);
}

void writeSettings(Setting &s){
  EEPROM.put(EEPROM_START_ADDR, s);
  EEPROM.commit();
}