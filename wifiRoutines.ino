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
    wifi.setDeviceID(PSTR("Bitponics"));
    wifi.save();
    resetBoard(); 
  }
  Serial.println();
}

//********************************************************************************

bool associateWifi(){

  Serial.print(F("-> Connecting to wifi... "));

  if (!wifi.isAssociated()){

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
  Serial.print(F("-> Scanning networks... "));
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
  Serial.println(F("-> Setting up AP... "));
  bwifiSet = false;

  //      wifi.setAuth(WIFLY_WLAN_AUTH_OPEN);
  //      wifi.setJoin(WIFLY_WLAN_JOIN_ADHOC);
  //      wifi.setDHCP(WIFLY_DHCP_MODE_AUTOIP);

  if(wifi.createAPNetwork("BitponicsSetup", 1)){
    Serial.println(F("created"));
    Serial.print(F("IP: "));
    Serial.println(wifi.getIP(addr, sizeof(addr)));

  } 
  else {

    Serial.print(F("failed"));
    //reboot
  }

  if (wifi.getPort() != 80) {
    wifi.setPort(80);
    /* local port does not take effect until the WiFly has rebooted (2.32+) */
    wifi.save();
    Serial.println(F("-> Set port to 80, rebooting"));
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

  Serial.print(F("-> creating hash... "));
  Sha256.initHmac((uint8_t*)SKEY,16); 
  Sha256.print(F("POST /api/devices/"));
  Sha256.print(MAC);
  Sha256.print(F("/status HTTP/1.1"));
  if(!calibMode[0]){
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
  else {
    Sha256.print(calibMode);
    Sha256.print(F(","));
    Sha256.print(F("success"));
  }
  Sha256.print(water);
  hash = Sha256.resultHmac(); //must save hash to use
  Serial.println(F("done"));

  Serial.print(F("-> opening connection... "));
  if (wifi.open(site, 80, RESET)) {

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
      if(calibMode[0]) Serial.println(F("X-Bpn-Mode: calib"));
      Serial.println(F("Connection: close"));
      Serial.println();

      if(!calibMode[0]){
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
      else {
        Serial.print(calibMode);
        Serial.print(F(","));
        Serial.print(F("success"));
      }


      Serial.println(); 
      Serial.println(); 

    }
    Serial.print(F("-> Sending request... "));

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
    if(calibMode[0]) wifi.println(F("X-Bpn-Mode: calib"));
    wifi.println(F("Connection: close"));
    wifi.println();

    if(!calibMode[0]){
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
    else {
      wifi.sendChunk(calibMode);
      wifi.sendChunk(F(","));
      wifi.sendChunk(F("success"));
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
  // Serial.println(F("-> Saving network"));

  if(wifi.setAuth(WIFLY_WLAN_AUTH_OPEN)) Serial.println(F("Set WPA Auth")); // TODO check if this is correct
  if(wifi.setPassphrase(pass)) Serial.println(F("Set Pass"));
  if(wifi.setDeviceID("Station")) Serial.print(F("Set DeviceID "));
  Serial.println(wifi.getDeviceID(deviceId,sizeof(deviceId)));

  if( wifi.save() ) Serial.println(F("-> Saved"));
  if(wifi.setSSID(ssid)) Serial.print(F("-> Set SSID  "));
  Serial.println(ssid);

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

    Serial.print(F("IP: "));
    Serial.println(wifi.getIP(addr, sizeof(addr))); // TODO get rid of buf
    Serial.print(F("Netmask: "));
    Serial.println(wifi.getNetmask(addr, sizeof(addr)));
    Serial.print(F("Gateway: "));
    Serial.println(wifi.getGateway(addr, sizeof(addr)));

    basicAuthConnect(PSTR("POST"),PSTR("status"), true);

  }
  else {
    Serial.println(F("Connection Failed"));
    WIFI_STATE = WIFI_UNSET;
    wifiAp(); //set Reset Adhoc Mode
  }

}






