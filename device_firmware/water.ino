#define SERIESRESISTOR 2000    
#define SENSORPIN A6 
float height;
float range = 220;
float minVal = 190;
float maxHeight = 21;

//-----------------------------------------------------------------------------------------
void setupPh(unsigned int DATABAUD){

  Serial2.begin(DATABAUD);
  Serial2.setTimeout(6000);
  Serial2.print(F("L0\r"));
  delay(20);
  Serial2.print(F("L0\r"));
  delay(500);
  Serial.println(F("- pH"));
}

//-----------------------------------------------------------------------------------------
//function to ask for pH and return as float
void getPh(){
  Serial.print(F("pH...          "));
  Serial2.print(water);
  Serial2.print(F("\r")); //ask device
  delay(500);
  dtostrf(Serial2.parseFloat(),4,2,ph);
  Serial.println(ph);
}

//-----------------------------------------------------------------------------------------
void setupEc(unsigned int DATABAUD){

  Serial3.begin(DATABAUD);
  Serial3.setTimeout(6000);
  Serial3.print(F("L1\r"));
  delay(300);
  Serial3.print(F("L0\r"));
  delay(300);
  Serial3.print(F("P,1\r")); // set sensor type to K 0.1
  delay(300);
  Serial3.print(F("r\r")); // read value
  delay(300);
  while(Serial3.available()>0) Serial3.read();
  Serial.println(F("- EC"));
}

//-----------------------------------------------------------------------------------------
void getEc(){
  Serial.print(F("EC...          "));
  Serial3.print(water);
  Serial3.print(F("\r")); //ask device
  int count=0;
  boolean bValSet = false;
  while(Serial3.available()==0) {
    delay(100);
    count++;
    if(count>50){
      itoa(0,ec,10);
      Serial.print(F("EC Error: "));
      break; 
    }
  } 
 
  while(Serial3.available()>0){
    if(!bValSet) {
      itoa(Serial3.parseInt(),ec,10);
      bValSet=true;
    }
    else Serial3.read();
  }
  Serial.println(ec);

}

void getWaterLevel(){
  Serial.print(F("water level... "));
  dtostrf(maxHeight-(analogRead(SENSORPIN)-minVal)/range*maxHeight, 4,2,wl);
  Serial.println(wl);
}



