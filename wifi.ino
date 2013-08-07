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
void wifiConnect(char *ssid, char *pass, char *mode);
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
  if (!wifi.begin(&Serial1, &Serial)) {
    Serial.println(F("Failed to start wifi"));
    //terminal();
  }

  Serial.print(F("SSID:        "));
  wifi.getSSID(ssid, sizeof(ssid));
  Serial.println(ssid);
  if(strcasecmp(ssid, deviceId) == 0 || strcasecmp(ssid, "roving1") == 0 ){
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
      // Serial.println(F("-> Association attempt failed")); 
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


/// Reset network
void checkBtn(){
  boolean btnState = digitalRead(BUTTON);
  long btnStartTime = millis();
  long btnTime = 0;

  while(!btnState){
    btnTime = millis() - btnStartTime;
    btnState = digitalRead(BUTTON);

    if(btnTime > 3000){
      // change network name and reset
      Serial.println(F("button hard reset")); 
      Serial.println(F("-> Setting AP Mode"));
      wifi.setDeviceID(deviceId);
      wifi.save();
      resetBoard();
    }
    else if(btnTime > 1){
      resetBoard();
    }
  }

}

//********************************************************************************

void wifiLoop(){
  //Serial.println("in loop");
  // delay(100);
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
      //printMem();
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

