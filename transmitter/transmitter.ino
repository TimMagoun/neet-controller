#define DISP_TELEMETRY 0
#define DISP_CONTROLLER_INPUT 1
#define DISP_NONE 2
#define EEPROM_START_ADDR 0 // Only change if we need to write to a different eeprom block, lifespan 100k writes

#include "NEET_RF24.h"
#include <EEPROM.h>
#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>

NEET_RF24 radio(9, 10, 121, true);
char buf[MAX_TELEM_STRING_LEN + 1];
ControlInput in;

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

bool in_menu = false; // Tracks if menu is currently active (not idling)
// Main menu
MENU(mainMenu, "Main menu", doNothing, noEvent, wrapStyle
  ,SUBMENU(setRadio)
  ,SUBMENU(setDispOpt)
  ,FIELD(s.channel, "Channel", "", 0, 125, 1, 1, saveSetting, exitEvent, wrapStyle)
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
  // Serial.println(s.channel);
  // Serial.println(s.dispOpt);
  writeSettings(s);
  return proceed;
}

result idle(menuOut &o, idleEvent e) {
  switch(e) {
    case idleStart:
      o.println("Exiting menu, press * to enter");
      in_menu = false;
      break;
    // case idling:o.println("suspended...");break;
    case idleEnd:
      o.println("Entering menu");
      in_menu = true;
      nav.reset();
      break;
  }
  return proceed;
}

void setup(){
  Serial.begin(115200);
  Serial.println("[TX] Started transmitter");
  Serial.println("[TX] Use numbers or +- keys to navigate");
  Serial.println("[TX] Use * to enter menu and select");
  if (radio.begin()){
    Serial.println("[TX] Radio started");
  } else {
    Serial.println("[ERR] Radio not started, check wiring?");
    while(1);
  }
  Serial.flush();

  // Read settings from EEPROM
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

    cur_time = millis();
    if (radio_enabled && (cur_time - last_time) > (1000 / CONTROLLER_RATE_HZ)){
      readInputs(in);

      if (radio.txSendControlInput(in)){
        // Serial.println("Transmit success");
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
    } else {
      delay(1);
    }
  }
}

void readInputs(ControlInput &in){
  in.j1PotX = 0;
  in.j1PotY = 0;
  in.j1Button = false;
  in.j2PotX = 0;
  in.j2PotY = 0;
  in.j2Button = false;
  in.pot = 0;
  in.tSwitch = false;
  in.button1 = false;
  in.button2 = false;
  in.button3 = false;
  in.button4 = false;
}

void displayInputs(ControlInput &in){
  char buf[128];
  sprintf(buf, "Joy1: %d, %d | Joy2: %d, %d | Pot: %d | Switch: %d | J1Btn: %d | J2Btn: %d | Btns %d %d %d %d \n"
            , in.j1PotX, in.j1PotY, in.j2PotX, in.j2PotY, in.pot, in.tSwitch, in.j1Button, in.j2Button
            , in.button1, in.button2, in.button3, in.button4);
  Serial.print(buf);
}

void readSettings(Setting &s){
  s.dispOpt = EEPROM.read(EEPROM_START_ADDR);
  s.channel = EEPROM.read(EEPROM_START_ADDR + 1);
}

void writeSettings(Setting &s){
  EEPROM.update(EEPROM_START_ADDR, s.dispOpt);
  EEPROM.update(EEPROM_START_ADDR + 1, s.channel);
}