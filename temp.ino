/*-------------------------------------------*
              ONE WIRE
*-------------------------------------------*/
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 22 //pin
#define TEMPERATURE_PRECISION 12

OneWire onewire(ONE_WIRE_BUS);
DallasTemperature water(&onewire);
DeviceAddress water_temp;

/*-------------------------------------------*
            DTH-22 TEMP/HUM
*-------------------------------------------*/
#include "DHT.h"
#define DHTPIN 23     // pin
#define DHTTYPE DHT22   // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);

void setup_temps(){
  
  water.begin();
  dht.begin();
  Serial.print("Temperature Init...");
  Serial.print("Found ");
  Serial.print(water.getDeviceCount(), HEX);
  Serial.println(" devices.");
  Serial.println(); 
  
  if(!water.getAddress(water_temp,0)){
   Serial.println("Failed to get Address");
    
  } 
  water.requestTemperatures();
  float tempC = water.getTempC(water_temp);
  Serial.print("Water Temp: ");Serial.println(tempC);
  
}
//convert temp FLoat to Char
String tempChar(float t, char* buf){
  char* c = dtostrf(t,5,2,buf);
  return c;
}
//obsolete
//char* waterTempChar(char* buf){
//  float w = getWaterTemp();
//  return dtostrf(w,5,2,buf);
//  
//}
float getWaterTemp(){
 
  water.requestTemperatures();
  return water.getTempC(water_temp); 
  
}

float getAirTemp(){
  //error checking...
  float t = dht.readTemperature();
  if(isnan(t)) return NULL;
  else return t;
}

float getHumidity(){
  float h = dht.readHumidity();
  if(isnan(h)) return NULL;
  else return h;
}
