#include <Wire.h>
#include <TSL2561.h>
#include "customDataTypes.h"
void setup() {
  
  /* these are the required 
     BAUDS for scan to work */
  Serial.begin(115200); 
  wifiSetup(9600);
}

void loop(){
  
  wifiLoop();
  
  
}

void setup_sensors(){
  
  setup_temps();
  setup_light();
  
}
