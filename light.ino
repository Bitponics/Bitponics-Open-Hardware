#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561.h>
#define LUX_SDA 20
#define LUX_SCL 21

// via __ Adafruit Example for demonstrating the TSL2561 library - public domain!

Adafruit_TSL2561 tsl = Adafruit_TSL2561(TSL2561_ADDR_FLOAT, 12345);

//-------------------------------------------------------
void setup_light(){

  if(!tsl.begin()){
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.print("no Lux sensor detected");
    while(1);
  }
  else Serial.print("Light Sensor Ready...");

  tsl.enableAutoGain(true);          /* Auto-gain ... switches automatically between 1x and 16x */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */

}

//-------------------------------------------------------
float getLight(){
  /* Get a new sensor event */
  sensors_event_t event;
  tsl.getEvent(&event);

  if (event.light){
    return char(event.light);
  }
  return 0;

}
//-------------------------------------------------------


