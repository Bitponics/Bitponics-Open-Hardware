#define SERIESRESISTOR 2000    
#define SENSORPIN A6 
float height;
float range = 220;
float minVal = 190;
float maxHeight = 21;
//boolean waterLevel = false;

//-----------------------------------------------------------------------------------------
//init ph by making sure we can communicate
void setupPh(unsigned int DATABAUD){
  
 Serial2.begin(DATABAUD);
 Serial2.setTimeout(6000);
 Serial2.print(F("L0\r"));
 delay(20);
 Serial2.print(F("L0\r"));
 delay(500);
 //Serial2.print("L1\r");
 Serial.println(F("- pH"));
 
}
//-----------------------------------------------------------------------------------------
//function to ask for pH and return as float
void getPh(){
 //Serial.println("R\\r --> PH"); //let us know we are asking
 Serial.print(F("pH...          "));
 Serial2.print(water);
 Serial2.print(F("\r")); //ask device
 delay(500);
 dtostrf(Serial2.parseFloat(),4,2,ph);
 Serial.println(ph);
// ph = Serial2.parseFloat();
 //return Serial2.parseFloat(); //return data as float
}

//-----------------------------------------------------------------------------------------
void setupEc(unsigned int DATABAUD){
 
 Serial3.begin(DATABAUD);
 Serial3.setTimeout(6000);
 Serial3.print(F("L1\r"));
 delay(100);
 Serial3.print(F("L0\r"));
 delay(100);
 //Serial3.print("L1\r");
 // Set EC to K 0.1
 Serial3.print(F("P,1\r")); // set sensor type K
 Serial3.flush();
 Serial.println(F("- EC"));
}

//-----------------------------------------------------------------------------------------
void getEc(){
 //Serial.println("R\\r --> EC"); //let us know we are asking
 Serial.print(F("EC...          "));
 Serial3.print(water);
 Serial3.print(F("\r")); //ask device
 delay(500);
 
 //ec = Serial3.parseInt();
 itoa(Serial3.parseInt(),ec,10);
 Serial3.flush();
 Serial.println(ec);
// Serial3.parseInt();  // discard other readings
// Serial3.parseFloat();
 
// return ec;
 
}

void getWaterLevel(){
 Serial.print(F("water level... "));
 // int reading = analogRead(SENSORPIN);
 // wl = maxHeight-(analogRead(SENSORPIN)-minVal)/range*maxHeight;
  dtostrf(maxHeight-(analogRead(SENSORPIN)-minVal)/range*maxHeight, 4,2,wl);
  Serial.println(wl);
 // return height;
}


