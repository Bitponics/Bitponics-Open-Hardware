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
char data[256];
char opt[20];
boolean bwifiSet;
unsigned long time =0;
char* Networks;

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
  
  /* Uncomment to Test AdHoc Network// Setup */
  //wifi.setDeviceID("WiFly-GSX");
  
  Serial.print(F("DeviceID: "));
  String d= wifi.getDeviceID(buf, sizeof(buf));
  Serial.println(d);
  
  if(d.indexOf("WiFly-GSX")>0) WIFI_STATE = WIFI_UNSET;
  if(d.indexOf("AdhocServer")>0) WIFI_STATE = WIFI_UNSET;
  if(d.indexOf("WPAClient")>0) WIFI_STATE = WIFI_WPA;
  if(d.indexOf("WEPClient")>0) WIFI_STATE = WIFI_WEP;
  
  switch(WIFI_STATE){
    
    case(WIFI_UNSET):
        scanNetworks();
        wifiAdhoc();

    break;
 
    default:
        loadServerKeys();
        wifiAssociated();
    break;
    
  }
  
  if (wifi.isConnected()) {
    Serial.println(F("Old connection active. Closing"));
    wifi.close();
  }
      
  wifi.setProtocol(WIFLY_PROTOCOL_TCP);

  Serial.println(F("Ready"));
  
}

void loadServerKeys(){
    wifi.getFTPUSER(SKEY, sizeof(SKEY));
    Serial.print("SKEY: ");Serial.println(SKEY);
    wifi.getFTPPASS(PKEY, sizeof(PKEY));
    Serial.print("PKEY: ");Serial.println(PKEY);
  
  
}

//********************************************************************************
void wifiAssociated(){

  if (!wifi.isAssociated()){
	wifi.enableDHCP();
        
        Serial.print(F("DeviceID: "));
        Serial.println(wifi.getDeviceID(buf, sizeof(buf)));
        Serial.print(F("SSID: "));
        Serial.println(wifi.getSSID(buf, sizeof(buf)));
        
	if (wifi.join()) {
	    Serial.println("Joined wifi network");
	} else {
	    Serial.println("Failed to join wifi network");
	    wifiAdhoc();
	}
  }else {
    Serial.println(F("Device is associated."));

  }
  
}

//********************************************************************************
//********************************************************************************
void scanNetworks(){
   Serial.println(F("//////////////////////////////////////////////////////////////////"));
   Serial.println(F("Setting up Scan Mode..."));
   wifi.setJoin(WIFLY_WLAN_JOIN_ADHOC);
   Serial.println(F("Getting Networks"));
   int networks = wifi.getNumNetworks(buf,sizeof(buf)) ;
   Serial.print(F("Networks in Range: "));
   Serial.println(networks);
   //Serial.println(buf);
   Networks = wifi.getScanNew(data, sizeof(data), true);
   Serial.print(Networks);
   Serial.println(F("//////////////////////////////////////////////////////////////////"));
}

//********************************************************************************
void wifiAdhoc(){
      WIFI_STATE=WIFI_UNSET;
      Serial.println(F("Setting up Adhoc Mode..."));
      bwifiSet = false;
      wifi.setAuth(WIFLY_WLAN_AUTH_OPEN);
      wifi.setJoin(WIFLY_WLAN_JOIN_ADHOC);
      wifi.setDHCP(WIFLY_DHCP_MODE_AUTOIP);
      
      if(wifi.createAdhocNetwork(mySSID, 1)){
        Serial.println(F("Adhoc Created"));
        Serial.print(F("MAC: "));
        Serial.println(wifi.getMAC(buf, sizeof(buf)));
        Serial.print(F("IP: "));
        Serial.println(wifi.getIP(buf, sizeof(buf)));
      
      } else {
        
        Serial.print(F("Adhoc Failed"));
        
     }
     
    if (wifi.getPort() != 80) {
        wifi.setPort(80);
	/* local port does not take effect until the WiFly has rebooted (2.32) */
	wifi.save();
	Serial.println(F("Set port to 80, rebooting to make it work"));
	wifi.reboot();
	delay(3000);
    }
   
}

//********************************************************************************
//********************************************************************************
//********************************************************************************
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
            char ch = wifi.read();
            Serial.write(ch);
            if (ch == '\n') Serial.write('\r');
          }else{
           if(millis()>time){
             Serial.println(F("Making Server Call"));
             basicAuthConnect("PUT","sensor_logs");
             time = millis()+36000;
           }
            
          }
        break;
        case(WIFI_WEP):
        
          if (wifi.available() > 0) {
            char ch = wifi.read();
            Serial.write(ch);
            if (ch == '\n') Serial.write('\r');
          }else{
           if(millis()>time){
             Serial.println(F("Making Server Call"));
             basicAuthConnect("PUT","sensor_logs");
             time = millis()+36000;
           }
          }
        break;
//      default:
//      break;
        
      }//end wifi switch
   
}


//********************************************************************************
//********************************************************************************
void wifiAdhocRequestHandler(){
 
/* See if there is a request */
	if (wifi.gets(buf, sizeof(buf))) {
	    if (strncmp_P(buf, PSTR("GET / "), 6) == 0) {
		/* GET request */
		Serial.println(F("Got GET request"));
		while (wifi.gets(buf, sizeof(buf)) > 0) {
		      /* Skip rest of request */
		}
		//sendIndex();
                sendInitialJSON(); 
		Serial.println(F("Sent JSON "));
	    } else if (strncmp_P(buf, PSTR("POST"), 4) == 0) {
	        /* Form POST */
	        char ssid[32];
                char pass[32];
                char mode[16];
	        Serial.println(F("Got POST"));

		/* Get posted field value */
		if (wifi.match(F("SSID="))) wifi.getsTerm(ssid, sizeof(ssid),'\n');
                //else pass="error";
                Serial.println(ssid);
                if (wifi.match(F("PASS="))) wifi.getsTerm(pass, sizeof(pass),'\n');
                //else pass='error';
                Serial.println(pass);
                if (wifi.match(F("MODE="))) wifi.getsTerm(mode, sizeof(mode),'\n');
                //else mode='error';
                Serial.println(mode);
                if (wifi.match(F("SKEY="))) wifi.getsTerm(SKEY,sizeof(SKEY),'\n');
                //else _skey='error';
                Serial.println(SKEY);
                if (wifi.match(F("PKEY="))) wifi.gets(PKEY, sizeof(PKEY));
                //else _pkey = 'error'; 
                Serial.println(PKEY);

                if(wifi.setFtpUser(SKEY)) Serial.println(F("SAVED SKEY"));
                if(wifi.setFtpPassword(PKEY)) Serial.println(F("SAVED PKEY"));
                
                loadServerKeys();
                
                if(wifi.save() )Serial.println(F("Saving..."));;
                if(wifi.reboot())Serial.println(F("Rebooted."));
                
                loadServerKeys();                
                    
                sendConfirm(ssid, pass, mode, SKEY, PKEY);
              
		Serial.println(F("Sent greeting page"));
                wifiConnect(ssid, pass, mode);
		
	    } else {
	        /* Unexpected request */
		Serial.print(F("Unexpected: "));
		Serial.println(buf);
		wifi.flushRx();		// discard rest of input
		Serial.println(F("Sending 404"));
		send404();
	    }
	} 
  
}
//********************************************************************************
//********************************************************************************
void wifiConnect(char *ssid, char *pass, char *mode){
  /* Setup the wifi to store wifi network & passphrase */
  Serial.println(F("Saving network"));
  
  String m=mode;
  uint8_t i;
  if(m=="WPA_MODE"){ 
    
    if(wifi.setAuth(WIFLY_WLAN_AUTH_OPEN))Serial.println(F("Set WPA Auth"));
    if(wifi.setPassphrase(pass))Serial.println(F("Set Pass"));
    if(wifi.setDeviceID("Bitponics-WPAClient")) Serial.print(F("Set DeviceID "));
    Serial.println(wifi.getDeviceID(buf,sizeof(buf)));
    i=0;
  }else {
    if(wifi.setAuth(WIFLY_WLAN_AUTH_OPEN))Serial.println(F("Setting WEP Auth"));
    if(wifi.setKey(pass))Serial.println(F("Set Key"));
    if(wifi.setDeviceID("Bitponics-WEPClient")) Serial.print(F("Set DeviceID "));
    i=1;
  }
  
  if( wifi.save() )Serial.println(F("Saving..."));
  Serial.print(F("old SSID  ")); Serial.println(wifi.getSSID(buf,sizeof(buf)));
  if(wifi.setSSID(ssid)) Serial.print(F("Set SSID  "));
  Serial.println(wifi.getSSID(buf,sizeof(buf)));
  
  Serial.print(F("old JOIN  ")); Serial.println(wifi.getJoin(), DEC);
  if(wifi.setJoin(WIFLY_WLAN_JOIN_AUTO))Serial.print(F("Set JOIN  "));
  Serial.println(wifi.getJoin(), DEC);
  
  if(wifi.enableDHCP() )Serial.println(F("Enable DHCP"));
  if(wifi.save() )Serial.println(F("Saving..."));;
  if(wifi.reboot())Serial.println(F("Rebooted."));
  
  wifi.flushRx();
  if (wifi.isConnected()) {
      Serial.println(F("Old connection active. Closing"));
      wifi.close();
  }
  
  Serial.println(F("Attempting Connection"));
  if(wifi.join(ssid, pass, i)){
    Serial.println(F("Connection Success"));
    
    if(m=="WPA_MODE") WIFI_STATE = WIFI_WPA;
    else WIFI_STATE = WIFI_WEP;
    
    Serial.print("MAC: ");
    Serial.println(wifi.getMAC(buf, sizeof(buf)));
    Serial.print("IP: ");
    Serial.println(wifi.getIP(buf, sizeof(buf)));
    Serial.print("Netmask: ");
    Serial.println(wifi.getNetmask(buf, sizeof(buf)));
    Serial.print("Gateway: ");
    Serial.println(wifi.getGateway(buf, sizeof(buf)));
    //device id test
    Serial.print("DeviceID: ");
    Serial.println(wifi.getDeviceID(buf, sizeof(buf)));
   
    basicAuthConnect("PUT","sensor_logs");
    //basicConnection();
    
  }else {
    Serial.println(F("Connection Failed"));
    WIFI_STATE = WIFI_UNSET;
    wifiAdhoc(); //set Reset Adhoc Mode
  }

}
//********************************************************************************
//********************************************************************************
/**

  TODO: MAKE DATA CALLS HERE AND FORMULATE JSON STRING

*/
char* makeJson(char* b, int s){
    // will take variables in addition to a buffer and create a data string for the server.
    //sprintf(buf,"{}",_type,mac,_route);
    b = "{ \"light\":\"3232.32\", \"air\": \" 47.5\", \"water\":\" 37.5\" }";
    return b;
  
}
//********************************************************************************
//********************************************************************************
/** 
    Print key:hash to server
*/
void wifiAuthHeader(char* _pkey, uint8_t* hash) {
  wifi.print(_pkey);
  wifi.print(F(":"));
  for (int i=0; i<32; i++) {
    wifi.print("0123456789abcdef"[hash[i]>>4]);
    wifi.print("0123456789abcdef"[hash[i]&0xf]);
  }
  wifi.println();
}

//********************************************************************************
//********************************************************************************
/** 
    Print Hash to Serial
*/
void printHash(uint8_t* hash) {
  int i;
  for (i=0; i<32; i++) {
    Serial.print("0123456789abcdef"[hash[i]>>4]);
    Serial.print("0123456789abcdef"[hash[i]&0xf]);
  }
  Serial.println();
}

//********************************************************************************
//********************************************************************************
/** 
    Make Basic Auth Connection with server using BPN_DEVICE Auth Protocol
*/
boolean basicAuthConnect(char* _type, char* _route){
  uint8_t* hash;
  uint32_t a;
  char *mac = wifi.getMAC(opt, sizeof(opt));
  macAddress(mac, MAC);
  char *json = makeJson(data, sizeof(data) ); //this is where we will make all of our data
  
  //format header route
  //devices
  sprintf(buf,"%s /api/devices/%s/%s HTTP/1.1",_type,MAC,_route);
  String path = buf; 
  Serial.print(path); Serial.println(json); //print data we are going to write
  Serial.println(SKEY);
  //create our SHA256 Hash
  Sha256.initHmac((uint8_t*)SKEY,16);//create hash with Secret/Private Key
  Sha256.print(path);
  Sha256.print(json);
  hash = Sha256.resultHmac(); //must save hash to use
  printHash(hash);
  Serial.println();
    
  if (wifi.open(site, 80)) {
    Serial.print("Connected to ");
    Serial.println(site);
    //connect to server 
    wifi.println(path);
    wifi.println(F("Accept: application/vnd.bitponics.v1.deviceText"));
    wifi.println(F("Content-Type: application/vnd.bitponics.v1.deviceText"));
    wifi.println(F("User-Agent: Bitponics-Device v0.1 (Arduino Mega)"));
    wifi.println(F("Host: bitponics-dev.herokuapp.com"));
    //wifi.println(F("Host: dev.bitponics.com"));
    wifi.println(F("Transfer-Encoding: chunked"));
    //Authorization: HMAC 16bytepublickey:hexEncodingOfHash
    wifi.print(F("Authorization: BPN_DEVICE "));wifiAuthHeader(PKEY,hash);
    wifi.println(F("Cache-Control: no-cache"));
    //wifi.print(F("Content-Length:"));wifi.println( strlen(json) );
    wifi.println(F("Connection: close"));
    wifi.println(); //end header
    wifi.sendChunk(json);
    wifi.sendChunkln();
    return true;
   }else{
    Serial.println("Failed to connect");
    return false;
   }
 
}

//********************************************************************************
//********************************************************************************
/** Get Device Mac Address without : 
    Loop three characters at a time skip 3rd character
    Requesting 3rd will throw out of bounds in last iteration of i */
char macAddress(char *_m, char a[]){
  int c =0;  
  for(int i = 0; i<strlen(_m); i+=3){
   for(int j=0; j<2;j++){
     a[c]=_m[i+j]; //Serial.println(_m[i+j]); 
     c++;
   }
  }
}

//********************************************************************************
//********************************************************************************
/** Send initial JSON to /setup
    Requires Networks = wifi.getScanNew(data, sizeof(data), true); 
    where true returns Data formated as json array */
void sendInitialJSON(){
  char *mac = wifi.getMAC(buf, sizeof(buf));
  wifi.println(F("HTTP/1.1 200 OK"));
  wifi.println(F("Content-Type: application/json"));
  wifi.println(F("Transfer-Encoding: chunked"));
  wifi.println(F("Access-Control-Allow-Origin: *"));
  wifi.println(F("Cache-Control: no-cache"));
  wifi.println(F("Connection: close"));
  wifi.println();
  
  wifi.sendChunk(F("{ \"mac\": \""));
  wifi.sendChunk(mac);
  wifi.sendChunkln(F("\", \"networks\": "));
  wifi.sendChunkln(Networks);
  wifi.sendChunkln(F("}"));
  wifi.sendChunkln();
}

//********************************************************************************
/** Send a confirmation of data recieved */
void sendConfirm(char *ssid, char *pass, char *mode, char *skey, char *pkey){
    
    wifi.println(F("HTTP/1.1 200 OK"));
    wifi.println(F("Content-Type: application/json"));
    wifi.println(F("Transfer-Encoding: chunked"));
    wifi.println(F("Access-Control-Allow-Origin: *"));
    wifi.println(F("Cache-Control: no-cache"));
    wifi.println(F("Connection: close"));
    wifi.println();
    //formatted as JSON
    wifi.sendChunk(F("{ \"ssid\": \""));
    wifi.sendChunk(ssid);
    wifi.sendChunk(F("\", \"pass\": \""));
    wifi.sendChunk(pass);
    wifi.sendChunk(F("\", \"mode\": \""));
    wifi.sendChunk(mode);
    wifi.sendChunk(F("\", \"skey\": \""));
    wifi.sendChunk(skey);
    wifi.sendChunk(F("\", \"pkey\": \""));
    wifi.sendChunk(pkey);
    wifi.sendChunkln(F("\" }"));
    wifi.sendChunkln();
    
}
