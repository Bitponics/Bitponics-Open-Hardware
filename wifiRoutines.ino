//********************************************************************************
void loadServerKeys(){
  wifi.getFTPUSER(SKEY, sizeof(SKEY));
  Serial.print(F("Private Key: "));
  Serial.println(SKEY);
  wifi.getFTPPASS(PKEY, sizeof(PKEY));
  Serial.print(F("Public Key:  "));
  Serial.println(PKEY);
  if(SKEY[0] == 'r'){
    Serial.println(F("-> Incorrect keys, reseting"));
    wifi.setDeviceID("Bitponics");
    wifi.save();
    resetBoard(); 
  }
  Serial.println();
}

//********************************************************************************

bool associateWifi(){
  
  Serial.print(F("-> Connecting to wifi... "));

  if (!wifi.isAssociated()){
    //wifi.enableDHCP();

    //    Serial.print(F("DeviceID: "));
    //    Serial.println(wifi.getDeviceID(buf, sizeof(buf)));
    //    Serial.print(F("SSID: "));
    //    Serial.println(wifi.getSSID(buf, sizeof(buf)));

    if (wifi.join()) {
      Serial.println(F("joined"));
      setColor(GREEN);
    } 
    else {
      Serial.println(F("failed to join"));
      return false;
    }
  } 
  else {
    Serial.println(F("already associated"));
    setColor(GREEN);
  }
  return true;
}

//********************************************************************************
void scanNetworks(){
  Serial.println(F("-> Scan Mode"));
  networks = wifi.getScanNew(data, sizeof(data), true);

  //if String networks has only [] find networks
  //  while(strlen(networks)<3){
  //    Serial.println("No networks Found, Checking Again.");
  //    networks = wifi.getScanNew(data, sizeof(data), true);
  //  }
  Serial.println(networks);
}

//********************************************************************************
void wifiAp(){
  WIFI_STATE=WIFI_UNSET;

  scanNetworks();
  Serial.println(F("-> Setting up AP Mode"));
  bwifiSet = false;

  //      wifi.setAuth(WIFLY_WLAN_AUTH_OPEN);
  //      wifi.setJoin(WIFLY_WLAN_JOIN_ADHOC);
  //      wifi.setDHCP(WIFLY_DHCP_MODE_AUTOIP);

  if(wifi.createAPNetwork("BITPONICS", 1)){
    Serial.println(F("-> AP Created"));
    Serial.print(F("IP: "));
    Serial.println(wifi.getIP(addr, sizeof(addr)));

  } 
  else {

    Serial.print(F("-> AP Failed"));
    //reboot
  }

  if (wifi.getPort() != 80) {
    wifi.setPort(80);
    /* local port does not take effect until the WiFly has rebooted (2.32+) */
    wifi.save();
    Serial.println(F("-> Set port to 80, rebooting..."));
    wifi.reboot();
    delay(3000);
    
  }

  Serial.println(F("-> AP Ready"));
  setColor(BLUE);

}

//********************************************************************************
/** 
 * Make Basic Auth Connection with server using BPN_DEVICE Auth Protocol
 * _type - GET, POST, PUT 
 * _route - the server location we are looking for assuming /api/devices/:deviceId
 * _bGetData - if we are sending JSON sensor data or not
 * 
 */

// TODO: come up with naming convention for global variables
// TODO: use Hungarian Notation
boolean basicAuthConnect(char* _type, char* _route, boolean _bGetData){ // TODO: Rename function

  uint8_t* hash;
  uint32_t a;

 // char* json;

  //if(_bGetData)

  //if(calibMode== "") json = makeJson(data, sizeof(data), false ); 
  // else json = makeJson(data, sizeof(data), true );

//  char path[48];
//  sprintf(path,"POST /api/devices/%s/status HTTP/1.1",MAC);  //format header route
  // String path = buf; 
  // Serial.println(path); 

  //get temp celcius as string
  //char fert = tempChar(getWaterTemp(), opt); 

  //  if(_bGetData){
  //    //Serial.println("JSON: "); 
  //    //Serial.println(json);
  //  } //print data we are going to write
  //  // Serial.println(SKEY);

  Serial.print(F("-> creating hash... "));
  //create our SHA256 Hash
  Sha256.initHmac((uint8_t*)SKEY,16); //create hash with Secret/Private Key
  //Sha256.print(path);
  Sha256.print(F("POST /api/devices/"));
  Sha256.print(MAC);
  Sha256.print(F("/status HTTP/1.1"));
  if(_bGetData){
    Sha256.print(lux);
    Sha256.print(F(","));
    Sha256.print(air);
    Sha256.print(F(","));
    Sha256.print(water);
    Sha256.print(F(","));
    Sha256.print(hum);
    Sha256.print(F(","));
    Sha256.print(ph);
    Sha256.print(F(","));
    Sha256.print(ec);
    Sha256.print(F(","));
    Sha256.print(wl);
  }
  Sha256.print(water);
  hash = Sha256.resultHmac(); //must save hash to use
  //printHash(hash);
  Serial.println(F("done"));

  Serial.print(F("-> opening connection... "));
  if (wifi.open(site, 80, RESET)) {
    //Serial.println(F("connected"));

    if(debug){
      Serial.println(F("--- REQUEST ---"));
      //// connect to server ////
      Serial.print(F("POST /api/devices/"));
      Serial.print(MAC);
      Serial.println(F("/status HTTP/1.1"));
      Serial.println(F("Host: www.bitponics.com"));
      Serial.println(F("User-Agent: Bitponics-Device v1 (Arduino Mega)"));
      Serial.println(F("Accept: application/vnd.bitponics.v1.deviceText"));
      Serial.println(F("Transfer-Encoding: chunked"));

      Serial.print(F("Authorization: BPN_DEVICE "));
      Serial.print(PKEY);
      Serial.print(F(":"));
      for (int i=0; i<32; i++) {
        Serial.print("0123456789abcdef"[hash[i]>>4]);
        Serial.print("0123456789abcdef"[hash[i]&0xf]);
      }
      Serial.println();
      Serial.print(F("X-Bpn-Fert: "));
      Serial.println(water);
      Serial.println(F("Content-Type: application/vnd.bitponics.v2.deviceText"));
      Serial.println(F("Cache-Control: no-cache"));
      Serial.println(F("Connection: close"));
      Serial.println();

      if(_bGetData){
        Serial.print(lux);
        Serial.print(F(","));
        Serial.print(air);
        Serial.print(F(","));
        Serial.print(water);
        Serial.print(F(","));
        Serial.print(hum);
        Serial.print(F(","));
        Serial.print(ph);
        Serial.print(F(","));
        Serial.print(ec);
        Serial.print(F(","));
        Serial.print(wl);
      }

      Serial.println(); 
      Serial.println(); 

    }
    Serial.print(F("-> Sending request... "));
    ///  ///

    wifi.print(F("POST /api/devices/"));
    wifi.print(MAC);
    wifi.println(F("/status HTTP/1.1"));
    wifi.println(F("Host: www.bitponics.com"));
    wifi.println(F("User-Agent: Bitponics-Device v1 (Arduino Mega)"));
    wifi.println(F("Accept: application/vnd.bitponics.v1.deviceText"));
    wifi.println(F("Transfer-Encoding: chunked"));

    wifi.print(F("Authorization: BPN_DEVICE "));
    wifi.print(PKEY);
    wifi.print(F(":"));
    for (int i=0; i<32; i++) {
      wifi.print("0123456789abcdef"[hash[i]>>4]);
      wifi.print("0123456789abcdef"[hash[i]&0xf]);
    }
    wifi.println();

    wifi.print(F("X-Bpn-Fert: "));
    wifi.println(water);
    wifi.println(F("Content-Type: application/vnd.bitponics.v2.deviceText"));
    wifi.println(F("Cache-Control: no-cache"));
    wifi.println(F("Connection: close"));
    wifi.println();

    if(_bGetData){
      wifi.sendChunk(lux);
      wifi.sendChunk(F(","));
      wifi.sendChunk(air);
      wifi.sendChunk(F(","));
      wifi.sendChunk(water);
      wifi.sendChunk(F(","));
      wifi.sendChunk(hum);
      wifi.sendChunk(F(","));
      wifi.sendChunk(ph);
      wifi.sendChunk(F(","));
      wifi.sendChunk(ec);
      wifi.sendChunk(F(","));
      wifi.sendChunk(wl);
    }

    wifi.sendChunkln(); 

    //// end body ////
    receiveTimeout = millis() + receiveWait;
    return true;
  }
  else{
    Serial.println(F("Failed to connect"));
    bReceivedStatus = true;
    return false;
  }

}
//********************************************************************************
void wifiConnect(char *ssid, char *pass, char *mode){
  /* Setup the wifi to store wifi network & passphrase */
  Serial.println(F("-> Saving network"));
  //String m = mode;
  //uint8_t i;
  //if(m=="WPA_MODE"){ 
  if(wifi.setAuth(WIFLY_WLAN_AUTH_OPEN)) Serial.println(F("Set WPA Auth")); // TODO check if this is correct
  if(wifi.setPassphrase(pass)) Serial.println(F("Set Pass"));
  if(wifi.setDeviceID("Station")) Serial.print(F("Set DeviceID "));
  Serial.println(wifi.getDeviceID(deviceId,sizeof(deviceId)));
  //i=0;
  //  }
  //  else {
  //    if(wifi.setAuth(WIFLY_WLAN_AUTH_OPEN))Serial.println(F("Setting WEP Auth"));
  //    if(wifi.setKey(pass))Serial.println(F("Set Key"));
  //    if(wifi.setDeviceID("Bitponics-WEPClient")) Serial.print(F("Set DeviceID "));
  //    i=1;
  //  }

  if( wifi.save() ) Serial.println(F("-> Saved"));
  //  Serial.print(F("old SSID  ")); 
  //  Serial.println(wifi.getSSID(buf,sizeof(buf)));
  if(wifi.setSSID(ssid)) Serial.print(F("-> Set SSID  "));
  Serial.println(ssid);

//  Serial.print(F("old JOIN  ")); 
//  Serial.println(wifi.getJoin(), DEC);
  if(wifi.setJoin(WIFLY_WLAN_JOIN_AUTO))Serial.print(F("-> Set JOIN  "));
  Serial.println(wifi.getJoin(), DEC);

  if(wifi.enableDHCP() )Serial.println(F("-> DHCP Enabled "));
  if(wifi.save() )Serial.println(F("-> Saved"));

  if(wifi.reboot())Serial.println(F("-> Rebooted"));

  wifi.flushRx();
  if (wifi.isConnected()) {
    Serial.println(F("Old connection active. Closing"));
    wifi.close();
  }

  Serial.println(F("-> Attempting Connection"));
  if(wifi.join(ssid, pass, 0)){
    Serial.println(F("-> Connection Success"));
    WIFI_STATE = WIFI_SET;
    //    if(m=="WPA_MODE") WIFI_STATE = WIFI_WPA;
    //    else WIFI_STATE = WIFI_WEP;

    Serial.print(F("IP: "));
    Serial.println(wifi.getIP(addr, sizeof(addr))); // TODO get rid of buf
    Serial.print(F("Netmask: "));
    Serial.println(wifi.getNetmask(addr, sizeof(addr)));
    Serial.print(F("Gateway: "));
    Serial.println(wifi.getGateway(addr, sizeof(addr)));
    //device id test
//    Serial.print(F("DeviceID: "));
//    Serial.println(wifi.getDeviceID(deviceId, sizeof(addr)));

    basicAuthConnect("POST","status", true);

  }
  else {
    Serial.println(F("Connection Failed"));
    WIFI_STATE = WIFI_UNSET;
    wifiAp(); //set Reset Adhoc Mode
  }

}
//********************************************************************************
//********************************************************************************
//char* makeJson(char* b, int s, boolean calib){
//  // will take variables in addition to a buffer and create a data string for the server.
//  String json;
//
//  if(calib){
//    json += "{\"calib\":{\"mode\":\"";
//    json += calibMode;
//    json += "\",\"status\":\"success\"";
//  }
//  else{
//    Serial.println(F("Taking Readings and Making JSON"));
//    Serial.println();
//
//    Serial.println("-lux");
//    json += "{\"sensors\":{\"lux\":"; // TODO: check if string can be prestored in prgm mem
//    json += tempChar(getLight(),opt);
//
//    Serial.println("-air temp");
//    json += ",\"air\":";
//    tempChar(getAirTemp(), opt);
//    String air = opt;
//    json+=air;
//    //Serial.print("air temp: ");Serial.println(air);
//
//    Serial.println("-water temp");
//    json+= ",\"water\":";
//    float waterTemp = getWaterTemp();
//    tempChar(waterTemp,opt);
//    String water = opt;
//    json+=water;
//    //Serial.print("water temp: ");Serial.println(water);
//
//    Serial.println("-humidity");
//    json+=",\"hum\":";
//    tempChar(getHumidity(),opt);
//    String hum = opt;
//    json+=hum;
//    //Serial.print("humidity: ");Serial.println(hum);
//
//    Serial.println("-ec");
//    int ec = getEc(waterTemp);
//    json+=",\"ec\":";
//    json+= ec;
//
//    Serial.println("-ph");
//    tempChar(getPh(waterTemp),opt);
//    String ph = opt;
//    json+=",\"ph\":";
//    json+=ph;
//
//    //if(waterLevel){
//    Serial.println("-water level");
//    tempChar(getWaterLevel(),opt);
//    String wl = opt;
//    json+=",\"wl\":";
//    json+=wl;
//    //}
//  }
//
//  json+="}}";
//  //Serial.println(json);
//  json.toCharArray(b, s);
//
//  return b;
//
//}


//********************************************************************************
//********************************************************************************
/** 
 * Print key:hash to server
 */
//void wifiAuthHeader(char* _pkey, uint8_t* hash) {
//
//  wifi.print(PKEY);
//  wifi.print(F(":"));
//  for (int i=0; i<32; i++) {
//    wifi.print("0123456789abcdef"[hash[i]>>4]);
//    wifi.print("0123456789abcdef"[hash[i]&0xf]);
//  }
//  wifi.println();
//}

//********************************************************************************
//********************************************************************************
/** 
 * Print Hash to Serial
 */
//void printHash(uint8_t* hash) {
//  Serial.print(PKEY);
//  Serial.print(F(":"));
//  for (int i=0; i<32; i++) {
//    Serial.print("0123456789abcdef"[hash[i]>>4]);
//    Serial.print("0123456789abcdef"[hash[i]&0xf]);
//  }
//  Serial.println();
//}
//
//
////********************************************************************************
//
//void printMem(){
//  Serial.println();
//  Serial.print(F("Free Memory:   "));
//  Serial.println(wifi.getFreeMemory(),DEC);
//  Serial.println();
//
//};
//
//






