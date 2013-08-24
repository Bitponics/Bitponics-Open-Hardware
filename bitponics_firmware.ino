//global includes
#include <Wire.h>
//#include "customDataTypes.h"
#include <avr/wdt.h>

const byte RED[] = {
  255, 0,0};
const byte YELLOW[] = {
  255, 255,0};
const byte ORANGE[] = {
  255,124,0};
const byte BLUE[] = {
  0,0,255};
const byte GREEN[] = {
  0,255,0};
const byte PURPLE[] = {
  255,0,255};


#define RESET 7
#define WIFI_RESET 38
#define BUTTON A2

char addr[16];
char buf[80];
long timeout;
unsigned long reset_time = 1200000; //20 minutes for reset
unsigned long requestCount = 0;
int terminalMode;

int errors = 0;
int errMax = 5;
boolean setupSequence = true;
boolean debug = false;

unsigned int completedPosts = 0;
unsigned int failedPosts = 0;

//*********************  Sensor Variables  ************************ **************
char ec[5];
char lux[6];
char air[7];
char water[7];
char hum[7];
char ph[6];
char wl[6];
//********************************************************************************

void setup() {
  setupLED();
  setupWDT();

  digitalWrite(WIFI_RESET, HIGH);
  digitalWrite(RESET, HIGH);
  pinMode(RESET, OUTPUT);
  pinMode(BUTTON, INPUT);

  setupRelays();

  Serial.begin(38400);
  Serial.println();
  Serial.println(F("**********************"));
  Serial.println(F("-> Device Boot"));
}

void loop(){

  if(setupSequence && !terminalMode){
    setupSensors(38400);
    if(!digitalRead(BUTTON)) checkTerm();
    if(!terminalMode){
      setupWifi(9600);
      setupSequence = false;
    }
  }

  checkBtn();
  if(!terminalMode) wifiLoop();
  if(millis()>reset_time) resetBoard();
}

void checkTerm(){
  Serial.println(F("Mode Switch"));
  int pressCount = 0;
  int startTime = millis();
  boolean lastState;
  while(startTime+3000>millis()){
    boolean curState = digitalRead(BUTTON);
    if(lastState!=curState){
      pressCount++; 
      Serial.println(pressCount);
    }
    if (pressCount>4){
      Serial.println(F("term mode"));
      terminalMode = true;
      break;
    }
    lastState = curState;
  }
  Serial.println(F("end mode switch"));

}

void setupSensors(unsigned int DATABAUD){
  Serial.println(F("-> Initalizing sensors"));
  setupTemps(); // TODO: change to setupTempAndHumidity();
  setupLight();
  setupEc(DATABAUD);
  setupPh(DATABAUD);
}


void resetBoard(){
  //wdt_enable(WDTO_8S); 
  Serial.println(F("-> Board reset"));
  resetWifi();
  wdt_enable(WDTO_30MS); 
  delay(1000);
}

void setupWDT() {
  cli();
  MCUSR = 0;
  WDTCSR |= B00011000;
  WDTCSR = B01000111;
  sei();
}


ISR(WDT_vect){
  if(millis()-timeout > 45000 && requestCount > 0){
    resetBoard(); 
  }
}
