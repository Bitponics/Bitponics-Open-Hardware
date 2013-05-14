//global includes
#include <Wire.h>
#include "customDataTypes.h"
#include <avr/wdt.h>

#define RESET 7
#define WIFI_RESET 38
#define BUTTON A2

unsigned long reset_time = 1200000; //20 minutes for reset

void setup() {
  //pinMode( );
  digitalWrite(WIFI_RESET, HIGH);
  digitalWrite(RESET, HIGH);
  pinMode(RESET, OUTPUT);
 // setupEEPROM();
  setupRelays();
  setupLedPins();

  pinMode(BUTTON, INPUT);
  //attachInterrupt(0, reset, RISING);

  /* these are the required 
   BAUDS for scan to work */
  Serial.begin(115200); 
  wifiSetup(9600);

}

void loop(){

  wifiLoop();
 // cycleLoop();
  terminal();
  //if(millis()>reset_time) resetBoard();
}

void setup_sensors(unsigned int DATABAUD){

  setup_temps();
  setup_light();
  setup_ec(DATABAUD);
  setup_ph(DATABAUD);

}


void terminal(){

  while(Serial1.available() > 0) {
    Serial.write(Serial1.read());
  }

  if(Serial.available()) {
    Serial1.write(Serial.read());
  }

}

void reset(){
  //wdt_enable(WDTO_8S); 
  wdt_enable(WDTO_30MS); 
  delay(1000);
}

