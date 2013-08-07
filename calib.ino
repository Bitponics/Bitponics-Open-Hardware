#define PH_CAL_TIME 2000
#define EC_CAL_TIME 2000

String calibMode = "";

void calibrate(){
  setColor(YELLOW);
  //long curTime;
  // PH *****
  if(calibMode == "ph_7"){
    Serial.println(F("ph_7 calib begun"));
    Serial2.print(F("C\r")); // set continous mode
    delay(PH_CAL_TIME);   // wait for stable
    Serial2.flush();      // flush serial
    Serial2.print(F("S\r")); // take calib reading
    Serial2.print(F("E\r")); // set in idle
    Serial.println(F("ph_7 calib complete"));
  } 
  if(calibMode == "ph_4"){
    Serial.println(F("ph_4 calib begun"));
    Serial2.print(F("C\r")); // set continous mode
    delay(PH_CAL_TIME);   // wait for stable
    Serial2.flush();      // flush serial
    Serial2.print(F("F\r")); // take calib reading
    Serial2.print(F("E\r")); // set in idle
    Serial.println(F("ph_4 calib complete"));
  } 
  if(calibMode == "ph_10"){
    Serial.println(F("ph_10 calib begun"));
    delay(PH_CAL_TIME);   // wait for stable
    Serial2.print(F("C\r")); // set continous mode
    Serial2.print(F("T\r")); // take calib reading
    Serial2.print(F("E\r")); // set in idle
    Serial2.flush();      // empty serial buffer
    Serial.println(F("ph_10 calib complete"));
  } 

  // EC *****
  if(calibMode == "ec_dr"){
    Serial.println(F("dry calib begun"));
    Serial3.print(F("z0\r")); // take dry reading
    Serial.println(F("ec_dr calib complete"));
  } 
  if(calibMode == "ec_hi"){
    Serial.println(F("ec_hi calib begun"));
    Serial3.print(F("C\r"));  // set continous mode
    delay(EC_CAL_TIME);    // wait for stable
    Serial3.print(F("Z30\r"));// take hi reading
    Serial2.print(F("E\r")); // set in idle
    Serial.println(F("ec_hi calib complete"));
  } 
  if(calibMode == "ec_lo"){
    Serial.println(F("ec_lo calib begun"));
    delay(EC_CAL_TIME);   // wait for stable
    Serial3.print(F("Z2\r"));// take lo reading
    Serial2.print(F("E\r")); // set in idle
    Serial.println(F("ec_lo calib complete"));
  } 

}
