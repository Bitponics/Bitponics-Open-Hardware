//global includes
#include <Wire.h>
#include "customDataTypes.h"

void setup() {
  
  /* these are the required 
     BAUDS for scan to work */
  Serial.begin(115200); 
  setupRelayPins();
  wifiSetup(9600);
}

void loop(){
  
  wifiLoop();
  cycleLoop();
}

void setup_sensors(unsigned int DATABAUD){
  
  setup_temps();
  setup_light();
  setup_ec(DATABAUD);
  setup_ph(DATABAUD);
  
}
