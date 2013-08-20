#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561.h>
#define LUX_SDA 20
#define LUX_SCL 21

Adafruit_TSL2561 tsl = Adafruit_TSL2561(TSL2561_ADDR_FLOAT, 12345);

//-------------------------------------------------------
void setupLight(){

  if(!tsl.begin()){
    Serial.print(F("no lux sensor detected"));
    while(1);
  }
  else Serial.println(F("- lux Sensor"));

  tsl.enableAutoGain(true);          /* Auto-gain ... switches automatically between 1x and 16x */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);      /* fast but low resolution */

}

//-------------------------------------------------------
void getLight(){

  Serial.print(F("light...       "));
  sensors_event_t event;

    tsl.getEvent(&event);
    //lux = event.light;
    itoa(event.light,lux,10);
    Serial.println(lux);


}
//-------------------------------------------------------


