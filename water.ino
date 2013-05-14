//-----------------------------------------------------------------------------------------
//init ph by making sure we can communicate
void setup_ph(unsigned int DATABAUD){
  
 Serial2.begin(DATABAUD);
 Serial2.setTimeout(6000);
 Serial2.print("L1\r");
 delay(1000);
 Serial2.print("L0\r");
 delay(1000);
 Serial2.print("L1\r");
 
}
//-----------------------------------------------------------------------------------------
//function to ask for pH and return as float
float getPh(float _c){
 //Serial.println("R\\r --> PH"); //let us know we are asking
 Serial2.print(_c);
 Serial2.write(13); //ask device
 delay(500);
 return Serial2.parseFloat(); //return data as float
}

//-----------------------------------------------------------------------------------------
void setup_ec(unsigned int DATABAUD){
 
 Serial3.begin(DATABAUD);
 Serial3.setTimeout(6000);
 Serial3.print("L1\r");
 delay(1000);
 Serial3.print("L0\r");
 delay(1000);
 Serial3.print("L1\r");
 // Set EC to K 0.1
 Serial3.print("P,1\r"); // set sensor type K
 Serial3.flush();
}

//-----------------------------------------------------------------------------------------
EC getEc(float _c){
 EC t;
 //Serial.println("R\\r --> EC"); //let us know we are asking
 Serial3.print(_c);
 Serial3.print("\r"); //ask device
 delay(500);
 
 t.conductivity= Serial3.parseInt() ;
 t.tds = Serial3.parseInt();
 t.salinity = Serial3.parseFloat();
 
 return t;
 
}
//-----------------------------------------------------------------------------------------
void print_ec(EC &_t){
  Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  Serial.println("EC");
  Serial.print("Conductivity \t");
  Serial.println(_t.conductivity);
  Serial.print("tds \t");
  Serial.println(_t.tds);
  Serial.print("salinity \t");
  Serial.println(_t.salinity);
  Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  
}
