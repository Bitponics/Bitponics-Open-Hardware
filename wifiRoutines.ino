//********************************************************************************
void loadServerKeys(){
    wifi.getFTPUSER(SKEY, sizeof(SKEY));
    Serial.print("SKEY: ");Serial.println(SKEY);
    wifi.getFTPPASS(PKEY, sizeof(PKEY));
    Serial.print("PKEY: ");Serial.println(PKEY);
  
  
}

//********************************************************************************
bool associateWifi(){

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
     return false;
   }
 } else {
  Serial.println(F("Device is associated."));
}
return true;
}

//********************************************************************************
void scanNetworks(){
   Serial.println(F("//////////////////////////////////////////////////////////////////"));
   Serial.println(F("Setting up Scan Mode..."));
   Networks = wifi.getScanNew(data, sizeof(data), true);
   
   //if String networks has only [] find networks
   while(strlen(Networks)<3){
     Serial.println("No Networks Found. Checking Networks Again.");
     Networks = wifi.getScanNew(data, sizeof(data), true);
   }
   
   Serial.println(Networks);
   Serial.println(F("//////////////////////////////////////////////////////////////////"));
}

//********************************************************************************
void wifiAp(){
      WIFI_STATE=WIFI_UNSET;
      scanNetworks();
      Serial.println(F("Setting up AP Mode..."));
      bwifiSet = false;
      
//      wifi.setAuth(WIFLY_WLAN_AUTH_OPEN);
//      wifi.setJoin(WIFLY_WLAN_JOIN_ADHOC);
//      wifi.setDHCP(WIFLY_DHCP_MODE_AUTOIP);
      
      if(wifi.createAPNetwork(mySSID, 1)){
        Serial.println(F("Adhoc Created"));
        Serial.print(F("MAC: "));
        Serial.println(wifi.getMAC(buf, sizeof(buf)));
        Serial.print(F("IP: "));
        Serial.println(wifi.getIP(buf, sizeof(buf)));
      
      } else {
        
        Serial.print(F("AP Failed"));
        //reboot
     }
     
    if (wifi.getPort() != 80) {
        wifi.setPort(80);
	/* local port does not take effect until the WiFly has rebooted (2.32+) */
	wifi.save();
	Serial.println(F("Set port to 80, rebooting to make it work"));
	wifi.reboot();
	delay(3000);
    }
    
    Serial.println(F("AP Ready"));
   
}

//********************************************************************************
/** 
    Make Basic Auth Connection with server using BPN_DEVICE Auth Protocol
    _type - GET, POST, PUT 
    _route - the server location we are looking for assuming /api/devices/:deviceId
    _bGetData - if we are sending JSON sensor data or not
    
*/
boolean basicAuthConnect(char* _type, char* _route, boolean _bGetData){
  requestCount++;
  Serial.println();
  Serial.print("Request Count ");Serial.println(requestCount);
  Serial.println();
  uint8_t* hash;
  uint32_t a;
  char *mac = wifi.getMAC(opt, sizeof(opt));
  macAddress(mac, MAC);
  char* json;
  if(_bGetData)  json = makeJson(data, sizeof(data) ); //this is where we will make all of our data
  
  sprintf(buf,"%s /api/devices/%s/%s HTTP/1.1",_type,MAC,_route);  //format header route
  String path = buf; 
  Serial.println(path); 
  
  //get temp celcius as string
  String fert = tempChar(getWaterTemp(), opt); 
  Serial.print("String: ");Serial.println(fert);
  
  if(_bGetData){Serial.println("JSON: "); Serial.println(json);} //print data we are going to write
  Serial.println(SKEY);
  
  //create our SHA256 Hash
  Sha256.initHmac((uint8_t*)SKEY,16); //create hash with Secret/Private Key
  Sha256.print(path);
  if(_bGetData) Sha256.print(json);
  Sha256.print(fert);
  hash = Sha256.resultHmac(); //must save hash to use
  printHash(hash);
  Serial.println();
    
  if (wifi.open(site, 80, RESET)) {
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
    wifi.print(F("X-Bpn-Fert:"));//need to print out temp
    wifi.println(fert);
    wifi.println(F("Cache-Control: no-cache"));
    //wifi.print(F("Content-Length:"));wifi.println( strlen(json) );
    wifi.println(F("Connection: close"));
    wifi.println(); //end header
    if(_bGetData) wifi.sendChunk(json);
    wifi.sendChunkln(); //end body
    receiveTimeout= millis() + receiveWait;
    return true;
   }else{
    Serial.println("Failed to connect");
    bReceivedStatus = true; bReceivedSensor = true; bReceivedCycles = true;
    return false;
   }
 
}
//********************************************************************************
void wifiConnect(char *ssid, char *pass, char *mode){
  /* Setup the wifi to store wifi network & passphrase */
  Serial.println(F("Saving network"));
  setup_sensors(38400); ///eventually will need to set up all sensors
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
   
    basicAuthConnect("PUT","sensor-logs", true);
    
  }else {
    Serial.println(F("Connection Failed"));
    WIFI_STATE = WIFI_UNSET;
    wifiAp(); //set Reset Adhoc Mode
  }

}
//********************************************************************************
//********************************************************************************
char* makeJson(char* b, int s){
    // will take variables in addition to a buffer and create a data string for the server.
    Serial.println("Taking Readings and Making JSON");
    Serial.println();
    String json = "{\"air\":";
    tempChar(getAirTemp(), opt);
    String air = opt;
    json+=air;
    //Serial.print("air temp: ");Serial.println(air);
    json+= ",\"water\":";
    tempChar(getWaterTemp(),opt);
    String water = opt;
    json+=water;
    //Serial.print("water temp: ");Serial.println(water);
    json+=",\"hum\":";
    tempChar(getHumidity(),opt);
    String hum = opt;
    json+=hum;
    //Serial.print("humidity: ");Serial.println(hum);
    
    json+= ",\"lux\":";
    json+= tempChar(getLight(),opt);;
    
    EC ec = getEc(getWaterTemp());
    
    json+=",\"ec\":";
    json+=ec.conductivity;
    
    tempChar(getPh(getWaterTemp()),opt);
    String ph = opt;
    json+=",\"ph\":";
    json+=ph;
    
    json+="}";
    Serial.println(json);
    json.toCharArray(b, s);

    //b = "{ \"light\":\"3232.32\", \"air\": \" 47.5\", \"water\":\" 37.5\" }";
    return b;
  
}

//********************************************************************************
//********************************************************************************
/** Return Device Mac Address without : */
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

void printMem(){
       Serial.println();
       Serial.print(F("Free Memory:   "));
       Serial.println(wifi.getFreeMemory(),DEC);
       Serial.println();
  
};
