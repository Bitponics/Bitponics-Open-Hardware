/* Work around a bug with PROGMEM and PSTR where the compiler always
 * generates warnings.
 */
#undef PROGMEM 
#define PROGMEM __attribute__(( section(".progmem.data") )) 
#undef PSTR 
#define PSTR(s) (__extension__({static prog_char __c[] PROGMEM = (s); &__c[0];})) 

#define WIFI_UNSET  0
#define WIFI_WPA    1
#define WIFI_WEP    2

#include <WiFlyHQ.h>
#include "sha256.h"
#include "customDataTypes.h"

WiFly wifi;

const char mySSID[] = "BITPONICS";
//const char site[] = "dev.bitponics.com";
const char site[] = "bitponics-dev.herokuapp.com";

//Key Chars must have one extra space for 
char PKEY[17]; //Public Key Stored in Wifly FTP Pass
char SKEY[17]; //Private/Secret Key Stored in Wifly FTP User
char MAC[20];
byte WIFI_STATE;

void wifiSetup(unsigned int BAUD);
void wifiAdhocRequestHandler();
void wifiConnect(char *ssid, char *pass, char *mode);
boolean wifiConnection();
void wifiLoop();

char buf[80];
char data[200];
char opt[20];
boolean bwifiSet;
/* time stuff */
unsigned long time_sensor = 0;
unsigned long time_status = 0;
unsigned long sensorPutDelay = 300000;
unsigned long statusPutDelay = 0;
boolean bCycles;
char* Networks;
boolean bReceivedStatus = true;
boolean bReceivedSensor = true;
boolean bReceivedCycles = true;
unsigned long receiveTimeout;
unsigned long receiveWait = 36000;
unsigned long requestCount = 0;

//********************************************************************************
void wifiSetup(unsigned int BAUD) {
  
  Serial.println();
  Serial.println(F("Starting"));
  Serial.println(F("Free Memory: "));
  Serial.println(wifi.getFreeMemory(),DEC);
  
  Serial1.begin(BAUD);
  if (!wifi.begin(&Serial1, &Serial)) {
      Serial.println(F("Failed to start wifi"));
      //terminal();
  }
  
  Serial.print(F("DeviceID: "));
  Serial.println(wifi.getDeviceID(buf, sizeof(buf)));
  Serial.print(F("SSID: "));
  Serial.println(wifi.getSSID(buf, sizeof(buf)));
  
  /* Uncomment to Test AdHoc Network//Setup */
  //wifi.setDeviceID("WiFly-GSX");
  
  Serial.print(F("DeviceID: "));
  String d= wifi.getDeviceID(buf, sizeof(buf));
  Serial.println(d);
  
  if(d.indexOf("WiFly-GSX")>0) WIFI_STATE = WIFI_UNSET;
  if(d.indexOf("AdhocServer")>0) WIFI_STATE = WIFI_UNSET;
  if(d.indexOf("WPAClient")>0) WIFI_STATE = WIFI_WPA;
  if(d.indexOf("WEPClient")>0) WIFI_STATE = WIFI_WEP;
  wifi.setProtocol(WIFLY_PROTOCOL_TCP);
  switch(WIFI_STATE){
    
    case(WIFI_UNSET):
        wifiAdhoc();

    break;
 
    default:
        loadServerKeys();
        wifiAssociated();
        setup_sensors(38400); ///eventually will need to set up all sensors
        basicAuthConnect("GET","cycles", false);
        bReceivedCycles = false;
    break;
    
  }
     
}

//********************************************************************************

void wifiLoop(){
 
   
      switch(WIFI_STATE){
        
        case(WIFI_UNSET):
        
         if (wifi.available() > 0) {
          wifiAdhocRequestHandler();
          
         }

        break;
        case(WIFI_WPA):
        
          if (wifi.available() > 0) {
            wifiAssocRequestHandler();
            //char ch = wifi.read();
            //Serial.write(ch);
            //if (ch == '\n') Serial.write('\r');
          }else{
             if(millis()>time_sensor && bReceivedStatus == true && bReceivedSensor == true && bReceivedCycles == true ){
              bReceivedSensor = false;
              basicAuthConnect("PUT","sensor_logs", true);
              time_sensor = millis()+sensorPutDelay; 
              printMem();
             }
             
             if(millis()>time_status && bReceivedStatus == true && bReceivedSensor == true && bReceivedCycles == true ){
               bReceivedStatus = false;
               basicAuthConnect("GET","refresh_status", false);
               time_status = millis()+statusPutDelay;
               printMem();
             }
            
          }
        break;
        case(WIFI_WEP):
        
          if (wifi.available() > 0) {
              wifiAssocRequestHandler();
          }else{
             if(millis()> receiveTimeout){ bReceivedStatus = true; bReceivedSensor = true; bReceivedCycles = true; }
            
             if(millis()>time_sensor && bReceivedStatus == true && bReceivedSensor == true && bReceivedCycles == true ){
              bReceivedSensor = false;
              basicAuthConnect("PUT","sensor_logs", true);
              time_sensor = millis()+sensorPutDelay;
              printMem();
             }
             
             if(millis()>time_status && bReceivedStatus == true && bReceivedSensor == true && bReceivedCycles == true ){
               bReceivedStatus = false;
               basicAuthConnect("GET","refresh_status", false);
               time_status = millis()+statusPutDelay;
               printMem();     
             }

          }
        break;
//      default:
//      break;
        
      }//end wifi switch
}



