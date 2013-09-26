//********************************************************************************
//********************************************************************************

void wifiAssocRequestHandler(){

  char errorMsg[28];

  //Serial.println(buf);
  if(wifi.match(F("HTTP/1.1 200"))){

    //Serial.println(F("success!"));
    if (wifi.match(F("STATES="))){
      Serial.print(F("states = "));
      wifi.getsTerm(statesBuf, sizeof(statesBuf), '\n');
      Serial.println(statesBuf);
      int relayState[2] = {
        int(statesBuf[2]-'0'), int(statesBuf[6]-'0')
        };
        for(int i=0; i<2;i++){
          if(relayState[i] != curRelayState[i] || completedPosts == 0) switchRelay(i, relayState[i]);
        }
      completedPosts ++;
    }
    else if(wifi.match(F("CALIB_MODE="))){
      Serial.print(F("calib mode = "));
      wifi.getsTerm(calibBuf, sizeof(calibBuf),'\n');
      Serial.println(calibBuf);
      calibMode=calibBuf;
      calibrate();

    }
    else Serial.println(F("no update matches"));

  }
  else {
    Serial.print(F("error "));
    wifi.gets(errorMsg,sizeof(errorMsg));
    Serial.println(errorMsg);
  }

  Serial.println(F("------- Close -------"));

  timeout = millis();
  wifi.close();
  wifi.flushRx();
  bReceivedStatus = true;


}

//********************************************************************************
//********************************************************************************
void wifiApRequestHandler(){

  /* See if there is a request */
  /*
  
   switch(wifi.multiMatch_P(1000,2,F("GET"), F("POST"))){
   case -1:
   Serial.print(F("-> Unexpected Request"));
   wifi.flushRx();
   
   break;
   case 0:
   apGetResponse();
   break;
   case 1:
   apPostResponse();
   break;
   
   }
   
   */


  if (wifi.gets(buf, sizeof(buf))) {
    if (strncmp_P(buf, PSTR("GET"), 3) == 0) {
      apGetResponse(); 
    }
    else if (strncmp_P(buf, PSTR("OPEN*GET"), 8) == 0) {
      apGetResponse();
    }
    else if (strncmp_P(buf, PSTR("*OPEN*GET"), 9) == 0) {
      apGetResponse();
    }
    else if (strncmp_P(buf, PSTR("PEN*GET"), 7) == 0) {
      apGetResponse();
    }  
    else if (strncmp_P(buf, PSTR("POST"), 4) == 0) {
      apPostResponse();

    }
    else if (strncmp_P(buf, PSTR("OPEN*POST"), 9) == 0) {
      apPostResponse();

    } 
    else if (strncmp_P(buf, PSTR("PEN*POST"), 8) == 0) {
      apPostResponse();

    } 
    else if (strncmp_P(buf, PSTR("*OPEN*POST"), 10) == 0) {
      apPostResponse();

    }
    else {
      /* Unexpected request */
      Serial.print(F("Unexpected Request : "));
      Serial.println(buf);
      wifi.flushRx();		// discard rest of input
      Serial.println(F("Sending 404"));
      //send404();
    } 

  }
}

void apGetResponse(){

  Serial.println(F("-> Received GET request"));
  //  while (wifi.gets(buf, sizeof(buf)) > 0) {	      
  //    /* Skip rest of request */
  //    Serial.println(buf);
  //  }
  //sendIndex();
  sendInitialJSON();
  Serial.println(F("-> Sent JSON ")); 

}

void apPostResponse(){

  /* Form POST */
 // char ssid[32];
  char pass[32];
  char mode[16];
  Serial.println(F("-> Received POST"));

  /* Get posted field value */
  if (wifi.match(F("SSID="))) wifi.getsTerm(ssid, sizeof(ssid),'\n');
  if (wifi.match(F("PASS="))) wifi.getsTerm(pass, sizeof(pass),'\n');
  if (wifi.match(F("MODE="))) wifi.getsTerm(mode, sizeof(mode),'\n');
  if (wifi.match(F("SKEY="))) wifi.getsTerm(SKEY,sizeof(SKEY),'\n');
  if (wifi.match(F("PKEY="))) wifi.gets(PKEY, sizeof(PKEY));

  printHeaders();

    wifi.sendChunk(F("{ \"ssid\": \""));
    wifi.sendChunk(ssid);
    wifi.sendChunk(F("\", \"pass\": \""));
    wifi.sendChunk(pass);
    wifi.sendChunk(F("\", \"mode\": \""));
    wifi.sendChunk(mode);
    wifi.sendChunk(F("\", \"skey\": \""));
    wifi.sendChunk(SKEY);
    wifi.sendChunk(F("\", \"pkey\": \""));
    wifi.sendChunk(PKEY);
    wifi.sendChunkln(F("\" }"));
    wifi.sendChunkln();
    

  wifi.setFtpUser(SKEY);
  wifi.setFtpPassword(PKEY);

 // loadServerKeys();

  if(wifi.save() )Serial.println(F("-> Saving wifi data"));
  if(wifi.reboot())Serial.println(F("-> Rebooted wifi"));

  loadServerKeys();                

//  Serial.println(F("-> Sent greeting page"));
//  wifiConnect(ssid, pass, mode);

}

void printHeaders(){
      
    wifi.println(F("HTTP/1.1 200 OK"));
    wifi.println(F("Content-Type: application/json"));
    wifi.println(F("Transfer-Encoding: chunked"));
    wifi.println(F("Access-Control-Allow-Origin: *"));
    wifi.println(F("Cache-Control: no-cache"));
    wifi.println(F("Connection: close"));
    wifi.println();
}







