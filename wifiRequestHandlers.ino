//********************************************************************************
//********************************************************************************

void wifiAssocRequestHandler(){

  if (wifi.gets(buf, sizeof(buf))) {
    Serial.print(F("<------POST Response: "));

    if(strncmp_P(buf, PSTR("HTTP/1.1 200 OK"), 15) == 0 || strncmp_P(buf, PSTR("TTP/1.1 200 OK"), 14) == 0 || strncmp_P(buf, PSTR("TP/1.1 200 OK"), 13) == 0){ // for some reason sometimes it reads "HT" early
      Serial.println(F("Success------>"));
      Serial.println(F("HTTP/1.1 200 OK")); 
      if (wifi.match(F("Content-Type:"))){ 
        Serial.print(F("Content-Type: "));
        wifi.getsTerm(data, sizeof(data),'\n'); 
        Serial.println(data);
      }
      if (wifi.match(F("Set-Cookie"))){ 
        Serial.print(F("Set-Cookie: "));
        wifi.getsTerm(data, sizeof(data),'\n'); 
        Serial.print(data);
      }
      if (wifi.match(F("X-Bpn-Resourcename:"))){
        Serial.print(F("X-Bpn-Resourcename: ")); 
        wifi.gets(data, sizeof(data)); 
        Serial.println(data);
        String _pr = data; // our page resource from the server
        _pr.trim();
        if(_pr=="status"){
          if (wifi.match(F("Content-Length:"))){
            Serial.print(F("Content-Length: ")); 
            wifi.gets(data, sizeof(data)); 
            Serial.println(data);
          }
          if (wifi.match(F("Connection:"))){
            Serial.print(F("Connection: ")); 
            wifi.gets(data, sizeof(data)); 
            Serial.println(data);
          }
          

          if (wifi.match(F("STATES="))){
            Serial.print(F("STATES="));
            wifi.getsTerm(data, sizeof(data),';');
            Serial.println(data);
            String r = data;
            r.trim();
            //switchRelay(0, r.charAt(2)-'0');
            //switchRelay(1, r.charAt(6)-'0');
          }
          if(wifi.match(F("CALIB_MODE="))){
            Serial.print(F("CALIB_MODE="));
            wifi.getsTerm(data, sizeof(data),'\a');
            Serial.println(data);
            calibMode=data;
            calibrate();
          }
          else calibMode = "";
//          wifi.gets(data, sizeof(data));
//          Serial.println(data);

//          if(wifi.match(F("/a"))){
//            bReceivedStatus = true;
//            wifi.flushRx();		// discard rest of input
//
//            Serial.println(F("<------POST Success------>"));
//          }
//          else Serial.println(F("Error: no end byte"));

        }
      }
    } 
    else if(strncmp_P(buf,PSTR("HTTP/1.1 401 Unauthorized"),23)==0 ){
      //action item for unathorized server requests?
      Serial.println(F("Unauthorized------>"));
      Serial.println("HTTP/1.1 401 Unauthorized");
      wifi.gets(buf, sizeof(buf));
      Serial.println(buf);
      bReceivedStatus = true;
    } 
    else {
      Serial.println("Bad format------>");
      Serial.println(buf);
      wifi.gets(buf, sizeof(buf));
      Serial.println(buf);
      bReceivedStatus = true;
    }

  }
  
 // delay(4000); // why is this here?

  wifi.flushRx();
  bReceivedStatus = true;
 // if(wifi.isConnected()) wifi.close();
 // Serial.println("end close");

}

//********************************************************************************
//********************************************************************************
void wifiApRequestHandler(){

  /* See if there is a request */
  if (wifi.gets(buf, sizeof(buf))) {
    if (strncmp_P(buf, PSTR("GET"), 8) == 0) {
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
    else if (strncmp_P(buf, PSTR("POST"), 8) == 0) {
      apPostResponse();

    }
    else if (strncmp_P(buf, PSTR("OPEN*POST"), 10) == 0) {
      apPostResponse();

    } 
    else if (strncmp_P(buf, PSTR("PEN*POST"), 10) == 0) {
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

  Serial.println(F("Got GET request"));
  while (wifi.gets(buf, sizeof(buf)) > 0) {	      
    /* Skip rest of request */
    Serial.println(buf);
  }
  //sendIndex();
  sendInitialJSON();
  Serial.println(F("Sent JSON ")); 

}

void apPostResponse(){

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

  sendConfirm(ssid, pass, mode, SKEY, PKEY);

  if(wifi.setFtpUser(SKEY)) Serial.println(F("SAVED SKEY"));
  if(wifi.setFtpPassword(PKEY)) Serial.println(F("SAVED PKEY"));

  loadServerKeys();

  if(wifi.save() )Serial.println(F("Saving..."));
  if(wifi.reboot())Serial.println(F("Rebooted"));

  loadServerKeys();                

  Serial.println(F("Sent greeting page"));
  wifiConnect(ssid, pass, mode);



}


