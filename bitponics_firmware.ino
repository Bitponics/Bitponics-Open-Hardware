//global includes
#include <Wire.h>
#include "customDataTypes.h"

#define RESET 7

unsigned long reset_time = 1200000; //20 minutes for reset

void setup() {
  digitalWrite(RESET, HIGH);
  pinMode(RESET, OUTPUT);
  setupEEPROM();
  setupRelayPins();
  /* these are the required 
     BAUDS for scan to work */
  Serial.begin(115200); 
  wifiSetup(9600);
}

void loop(){
  
  wifiLoop();
  cycleLoop();
  
  if(millis()>reset_time) resetBoard();
}

void setup_sensors(unsigned int DATABAUD){
  
  setup_temps();
  setup_light();
  setup_ec(DATABAUD);
  setup_ph(DATABAUD);
  
}

