#include <lib/TSL2561/TSL2561.h>
#define LUX_SDA 20
#define LUX_SCL 21

// via __ Adafruit Example for demonstrating the TSL2561 library - public domain!

TSL2561 tsl(TSL2561_ADDR_FLOAT); 

//-------------------------------------------------------
void setup_light(){
  
  if (tsl.begin()) Serial.print("Light Sensor Ready...");
  //else _reset();
  delay(100);
  tsl.setGain(TSL2561_GAIN_16X);
  tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);
  
}

//-------------------------------------------------------
Light getLight(){
  Light l;

  l.lum = tsl.getFullLuminosity();
  l.ir = l.lum >> 16;
  l.full = l.lum & 0xFFFF;
  l.vis = l.full- l.ir;
  l.lux = tsl.calculateLux(l.full, l.ir);
  //printLight(l);
  return l;
    
}
//-------------------------------------------------------
void printLight( Light _l ){
  
  Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  Serial.print("Lum: ");Serial.print(_l.lum);   Serial.print("\t");
  delay(100);
  Serial.print("IR: "); Serial.print(_l.ir);   Serial.print("\t");
  delay(100);
  Serial.print("Full: "); Serial.print(_l.full);   Serial.print("\t");
  delay(100);
  Serial.print("Visible: "); Serial.print(_l.vis);   Serial.print("\t");
  delay(100);
  Serial.print("Lux: "); Serial.println(_l.lux);
  delay(100); 
  Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  
}
