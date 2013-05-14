//********************************************************************************
//********************************************************************************

void wifiAssocRequestHandler(){
  
    if (wifi.gets(buf, sizeof(buf))) {
      Serial.println(F("RECIEVED: "));
      if(strncmp_P(buf, PSTR("HTTP/1.1 200 OK"), 15) == 0 || strncmp_P(buf, PSTR("TP/1.1 200 OK"), 13) == 0){ // for some reason sometimes it reads "HT" early
            Serial.println(F("HTTP/1.1 200 OK"));
            if (wifi.match(F("Content-Type:"))){ Serial.print(F("Content-Type: "));wifi.getsTerm(data, sizeof(data),'\n'); Serial.println(data);}
            if (wifi.match(F("Set-Cookie"))){ Serial.print(F("Set-Cookie: "));wifi.getsTerm(data, sizeof(data),'\n'); Serial.println(data);}
            if (wifi.match(F("X-Bpn-Resourcename:"))){
              Serial.print(F("X-Bpn-Resourcename: ")); wifi.gets(data, sizeof(data)); Serial.println(data);
              String _pr = data; // our page resource from the server
              _pr.trim();
              if(_pr=="status"){
                
                Serial.println(F("<------Received Refresh Status------>"));
                if (wifi.match(F("Content-Length:"))){Serial.print(F("Content-Length: ")); wifi.gets(data, sizeof(data)); Serial.println(data);}
                if (wifi.match(F("Connection:"))){Serial.print(F("Connection: ")); wifi.gets(data, sizeof(data)); Serial.println(data);}
                wifi.gets(data, sizeof(data));
                Serial.println(data);
                //to do 
                //action item on REFRESH & OVERRIDES information
                if (wifi.match(F("STATES="))){
                   Serial.print(F("STATES="));wifi.getsTerm(data, sizeof(data),'\n');Serial.println(data);
                   String r = data;
                   r.trim();
//                   if(r == "1"){
//                      basicAuthConnect("GET","status", false);
//                      bReceivedCycles = false;
//                   }else
                
                   if(wifi.match(F("CALIB_MODE="))){Serial.print(F("CALIB_MODE="));wifi.getsTerm(data, sizeof(data),'\a');Serial.println(data);
                       // readCycles(data);
                       //readOverride(data);
                       
                   }
                }

                bReceivedStatus = true;
                wifi.flushRx();		// discard rest of input

                Serial.println("<------end receive status------>");
                
              }else if(_pr=="sensor-logs"){

                  wifi.flushRx();		// discard rest of input
                  bReceivedSensor = true;
              }
         }    
      } else if(strncmp_P(buf,PSTR("HTTP/1.1 401 Unauthorized"),23)==0 ){
        //action item for unathorized server requests?
        Serial.println(F("<------Received Unauthorized------>"));
        Serial.println("HTTP/1.1 401 Unauthorized");
        wifi.gets(buf, sizeof(buf));
        Serial.println(buf);
        bReceivedStatus = true;
      } else {
       Serial.println("<------Bad format------>");
       Serial.println(buf);
       wifi.gets(buf, sizeof(buf));
       Serial.println(buf);
       bReceivedStatus = true;
      }
      
    }
    delay(4000);
    Serial.println("getting nothing");
    if(wifi.isConnected()) wifi.close();
    wifi.flushRx();
    
}

//********************************************************************************
//********************************************************************************
void wifiApRequestHandler(){
 
/* See if there is a request */
	if (wifi.gets(buf, sizeof(buf))) {
  	   if (strncmp_P(buf, PSTR("GET"), 8) == 0) {
                  apGetResponse(); 
	    }else if (strncmp_P(buf, PSTR("OPEN*GET"), 8) == 0) {
                  apGetResponse();
	    }else if (strncmp_P(buf, PSTR("*OPEN*GET"), 9) == 0) {
                  apGetResponse();
	    }else if (strncmp_P(buf, PSTR("PEN*GET"), 7) == 0) {
                  apGetResponse();
	    }  
            else if (strncmp_P(buf, PSTR("POST"), 8) == 0) {
              apPostResponse();

            }else if (strncmp_P(buf, PSTR("OPEN*POST"), 10) == 0) {
              apPostResponse();

            } else if (strncmp_P(buf, PSTR("PEN*POST"), 10) == 0) {
              apPostResponse();

            } 
            else if (strncmp_P(buf, PSTR("*OPEN*POST"), 10) == 0) {
              apPostResponse();

            }else {
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
                
                if(wifi.save() )Serial.println(F("Saving..."));;
                if(wifi.reboot())Serial.println(F("Rebooted."));
                
                loadServerKeys();                
                    
		Serial.println(F("Sent greeting page"));
                wifiConnect(ssid, pass, mode);
		
	    

}
