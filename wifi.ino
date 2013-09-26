/* Work around a bug with PROGMEM and PSTR where the compiler always
 * generates warnings.
 */
#undef PROGMEM 
#define PROGMEM __attribute__(( section(".progmem.data") )) 
#undef PSTR 
#define PSTR(s) (__extension__({static prog_char __c[] PROGMEM = (s); &__c[0];})) 

#define WIFI_UNSET  0
#define WIFI_SET    1
//#define WIFI_WEP    2

#include <WiFlyHQ.h>
#include "sha256.h"
//#include "customDataTypes.h"

boolean lastBtnState = true;

WiFly wifi;

char deviceId[10] = "Bitponics";
const char site[] = "www.bitponics.com";

//Key Chars must have one extra space for `
char PKEY[17]; //Public Key Stored in Wifly FTP Pass
char SKEY[17]; //Private/Secret Key Stored in Wifly FTP User
char MAC[13];
byte WIFI_STATE;

void setupWifi(unsigned int BAUD);
void wifiApRequestHandler();
//void wifiConnect(char *ssid, char *pass, char *mode);
boolean wifiConnection();
void wifiLoop();

int associationAttemps = 0;

char ssid[33];

char data[200];
char statesBuf[16];
char calibBuf[18];
//char opt[20];
boolean bwifiSet;
/* time stuff */
//unsigned long time_sensor = 0;
unsigned long time_status = 0;
//unsigned long smaensorPutDelay = 300000;
const unsigned long statusPutDelay = 5000;
char* networks;
boolean bReceivedStatus = true;
unsigned long receiveTimeout;
unsigned long receiveWait = 36000;

//********************************************************************************
void setupWifi(unsigned int BAUD) {


  Serial.print(F("\nFree Memory: "));
  Serial.println(wifi.getFreeMemory(),DEC);

  Serial1.begin(BAUD);

  wifiUpdate();

  if (!wifi.begin(&Serial1, &Serial)) {
    Serial.println(F("Failed to start wifi"));
    //terminal();
  }
  checkBtn();

  Serial.print(F("SSID:        "));
  wifi.getSSID(ssid, sizeof(ssid));
  Serial.println(ssid);
  if(strcasecmp(ssid, deviceId) == 0 || strcasecmp(ssid, PSTR("roving1")) == 0 ){
    Serial.println(F("-> Setting AP Mode"));
    wifi.setDeviceID(deviceId);
    wifi.save();
  }
  macAddress(MAC);
  Serial.print(F("MAC:         "));
  Serial.println(MAC);


  Serial.print(F("WIFI Mode:   "));
  wifi.getDeviceID(deviceId, sizeof(deviceId));
  Serial.println(deviceId);

  if(deviceId[0] == 'B') WIFI_STATE = WIFI_UNSET;
  if(deviceId[0] == 'S') WIFI_STATE = WIFI_SET;

  wifi.setProtocol(WIFLY_PROTOCOL_TCP); // setup TCP protocol

  if(WIFI_STATE == WIFI_UNSET) {
    wifiAp(); 
  }
  else {
    setColor(ORANGE);
    loadServerKeys();
    while(!associateWifi()){
      associationAttemps++;
      if (associationAttemps > 10){
        resetWifi();
        resetBoard(); 
      }
    }

    //if(!associateWifi()) wifiAp();
  }
  timeout = millis();

}




//********************************************************************************

void wifiLoop(){
  delay(10);
  if(WIFI_STATE == WIFI_UNSET){
    if (wifi.available() > 0) {
      wifiApRequestHandler();
    }
  }
  if (WIFI_STATE == WIFI_SET){
    if (wifi.available() > 0) {  // check if anything in wifi buffer
      wifiAssocRequestHandler(); // handle wifi data
    }
    else if(millis()>time_status && bReceivedStatus == true){ // if last request was completed and timer elapsed, make a request
      if(calibMode == "") {
        Serial.println();
        Serial.println(F("---- Get sensors ----"));
        getSensors();   
        Serial.println(F("---------------------"));
        Serial.println();
      } 

      Serial.print(F("---- Request "));
      Serial.print(++requestCount);
      Serial.println(F(" ----"));
      bReceivedStatus = false; // reset status variable
      basicAuthConnect("POST","status", true); // standard status update post
      time_status = millis() + statusPutDelay; // reset POST timer
    }
  }
}

void getSensors(){
  getLight();
  getAirTemp();
  getWaterTemp();
  getHumidity();
  getPh();
  getEc();
  getWaterLevel();
}

void resetWifi(){
  digitalWrite(WIFI_RESET, LOW);
  delay(500);
  digitalWrite(WIFI_RESET, HIGH);
}

//********************************************************************************
//********************************************************************************
/** Return Device Mac Address without : */
void macAddress(char a[]){
  char macBuffer[18];
  wifi.getMAC(macBuffer, sizeof(macBuffer));
  int c=0;
  for(int i=0; i<18; i++){
    if(i%3 != 2){
      a[c] = macBuffer[i];
      c++;
    }
  }

}

void wifiUpdate(){
  delay(1000);
  Serial1.print(F("$$$"));
  delayTimeout();
  while(Serial1.available()>0){
    Serial1.read();
  }

  Serial1.print("\r");
  delayTimeout();
  if(Serial1.findUntil(PSTR("2.45"), PSTR("n"))) Serial.println(F("Wifi up to date"));
  else {
    Serial.print(F("Updating WIFI... "));
    Serial1.print(F("boot image 34\r"));
    delayTimeout();
    if(Serial1.findUntil(PSTR("34"), PSTR("OK"))){
      Serial1.print(F("reboot\r"));
      delayTimeout();
      if(!Serial1.find(PSTR("*Reboot*"))); //reset
      else Serial.println(F("success"));
    }
    else Serial.println(F("update failed"));
  } 
}

void delayTimeout(){
  int count = 0;
  while(Serial1.available()==0){
    delay(100);
    if(count++ > 50); // TODO reset 
  }
}

/// Reset network
void checkBtn(){

  if(!digitalRead(BUTTON)){
    for(int i=0; i<3; i++){
      setColor(BLACK);
      delay(500);
      setColor(BLUE);
      delay(500);
    }

    Serial.println(F("-> Setting AP Mode"));
    wifi.setDeviceID(PSTR("Bitponics"));
    wifi.setSSID(PSTR("Bitponics"));
    wifi.save();
    delay(2000);
    resetBoard();
  }

}


