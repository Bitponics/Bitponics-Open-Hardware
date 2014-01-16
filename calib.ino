const long phCalibTime = 180000;
const long ecCalibTime = 5000;

char calibMode[6] = "";

void calibrate(){
  setColor(YELLOW);

  Serial.println("Calibrating....");

  // PH *****
  if(!memcmp(calibMode,"ph_7", 4)){
    phCalib("S");
  } 
  else if(!memcmp(calibMode,"ph_4", 4)){
    phCalib("F");
  } 
  else if(!memcmp(calibMode,"ph_10", 5)){
    phCalib("T");
  } 

  // EC *****
  else if(!memcmp(calibMode,"ec_dr", 5)){
    Serial.println(F("dry calib begun"));
    Serial3.print(F("x\r")); // reset
    delay(3000);
    //while(Serial3.available() > 0) Serial.print(char(Serial3.read()));
    Serial3.print(F("l0\r"));
    delay(1000);
    Serial3.print(F("p,1\r")); // set probe type (k0.1)
    delay(1000);
    Serial3.print(water);
    Serial3.print(F("\r")); // set temp
    delay(1000);
    Serial3.print(F("z0\r")); // take dry reading
    delay(1000);
    Serial3.print(water);
    Serial3.print(F("\r")); // read calibrated
    delay(1000);
    while(Serial3.available() > 0)  Serial.print(char(Serial3.read()));
    Serial.println(F("ec_dr calib complete"));
  } 
  else if(!memcmp(calibMode,"ec_hi", 5)){
    ecCalib("30");
  } 
  else if(!memcmp(calibMode,"ec_lo", 5)){
    ecCalib("2");
  }
  else Serial.println(F("no match"));

}


void ecCalib(String ecCode){
  Serial.print(F("----"));
  Serial.print(calibMode);
  Serial.println(F(" calib begun----"));
  for(int i=0;i<4; i++) calibDelay();
  Serial3.print(F("Z"));
  Serial3.print(ecCode);
  Serial3.print(F("\r"));
  delay(1000);                // wait response
  while(Serial3.available() > 0)  Serial.print(char(Serial3.read()));  // read confirmation
  Serial.print(calibMode);
  Serial.println(F(" calib complete"));
  timeout = millis();
}

void calibDelay(){
  timeout = millis();
  delay(5000);
  timeout = millis(); 
}

void phCalib(String phCode){
  Serial.print(calibMode);
  Serial.println(F(" calib begun"));
  for(int i=0;i<2; i++) calibDelay();   // wait for stable
  Serial2.print(phCode); 
  Serial2.print(F("\r")); // take calib reading
  delay(1000);
  Serial2.print(water); // read
  Serial2.print(F("\r")); // read
  delay(1000);                // wait response
  while(Serial2.available() > 0)  Serial.print(char(Serial2.read()));  // read confirmation
  Serial.print(calibMode);
  Serial.println(F(" calib complete"));
}


