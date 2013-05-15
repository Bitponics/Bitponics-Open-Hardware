#define PH_CAL_TIME 2000
#define EC_CAL_TIME 2000

String calibMode = "";

void calibrate(){
  //long curTime;
  // PH *****
  if(calibMode == "ph_7"){
    Serial.println("ph_7 calib begun");
    Serial2.print("C\r"); // set continous mode
    delay(PH_CAL_TIME);   // wait for stable
    Serial2.flush();      // flush serial
    Serial2.print("S\r"); // take calib reading
    Serial2.print("E\r"); // set in idle
    Serial.println("ph_7 calib complete");
  } 
  if(calibMode == "ph_4"){
    Serial.println("ph_4 calib begun");
    Serial2.print("C\r"); // set continous mode
    delay(PH_CAL_TIME);   // wait for stable
    Serial2.flush();      // flush serial
    Serial2.print("F\r"); // take calib reading
    Serial2.print("E\r"); // set in idle
    Serial.println("ph_4 calib complete");
  } 
  if(calibMode == "ph_10"){
    Serial.println("ph_10 calib begun");
    delay(PH_CAL_TIME);   // wait for stable
    Serial2.print("C\r"); // set continous mode
    Serial2.print("T\r"); // take calib reading
    Serial2.print("E\r"); // set in idle
    Serial2.flush();      // empty serial buffer
    Serial.println("ph_10 calib complete");
  } 

  // EC *****
  if(calibMode == "ec_dr"){
    Serial.println("dry calib begun");
    Serial3.print("z0\r"); // take dry reading
    Serial.println("ec_dr calib complete");
  } 
  if(calibMode == "ec_hi"){
    Serial.println("ec_hi calib begun");
    Serial3.print("C\r");  // set continous mode
    delay(EC_CAL_TIME);    // wait for stable
    Serial3.print("Z30\r");// take hi reading
    Serial2.print("E\r"); // set in idle
    Serial.println("ec_hi calib complete");
  } 
  if(calibMode == "ec_lo"){
    Serial.println("ec_lo calib begun");
    delay(EC_CAL_TIME);   // wait for stable
    Serial3.print("Z2\r");// take lo reading
    Serial2.print("E\r"); // set in idle
    Serial.println("ec_lo calib complete");
  } 

}
