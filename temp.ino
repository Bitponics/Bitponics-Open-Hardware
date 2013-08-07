/*-------------------------------------------*
 ONE WIRE
 *-------------------------------------------*/
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 22 //pin
#define TEMPERATURE_PRECISION 12

OneWire onewire(ONE_WIRE_BUS);
DallasTemperature waterSensor(&onewire);
DeviceAddress waterTempAddress;

/*-------------------------------------------*
 DTH-22 TEMP/HUM
 *-------------------------------------------*/
#include "DHT.h"
#define DHTPIN 23       // pin
#define DHTTYPE DHT22   // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);

void setupTemps(){

  dht.begin();
  Serial.println("- DHT");
  waterSensor.begin();
  if(!waterSensor.getAddress(waterTempAddress,0)){
    Serial.println(F("Failed to setup water temp sensor"));
    //resetBoard();
  } 
  else Serial.println(F("- Water Temp"));

}

void getWaterTemp(){
  Serial.print(F("water temp...  "));
  
  waterSensor.requestTemperatures();
  //water = waterSensor.getTempC(waterTempAddress);
      dtostrf(waterSensor.getTempC(waterTempAddress),4,2,water);
  Serial.println(water);
  
//  return waterSensor.getTempC(waterTempAddress); 

}

void getAirTemp(){
  Serial.print(F("air temp...    "));
  errors= 0;
  //air = dht.readTemperature();
  dtostrf(dht.readTemperature(),4,2,air);
//  while(isnan(air)){
//    errors++;
//    if(errors > errMax) {
//      Serial.println(F("temperature read error... reseting!"));
//      resetBoard(); 
//    }
//  }

Serial.println(air);
}

void getHumidity(){
 // float h = dht.readHumidity();
// hum = dht.readHumidity();
Serial.print(F("humidity...    "));
  dtostrf(dht.readHumidity(),4,2,hum);
Serial.println(hum);

//  while(isnan(h)){
//    errors++;
//    if(errors > errMax) {
//      Serial.println(F("humididty read error... reseting!"));
//      resetBoard(); 
//    }
//  }
 // return h;
}


