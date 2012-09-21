//Custom Datatype for Bitponics project

#ifndef customDataTypes_H
#define customDataTypes_H

struct Value {
  int val;
  long int dur; 
    
};

struct  Cycle{
  int ID;
  int start;
  Value live;
  Value rest;
};

struct EC{

   long int conductivity;
   long int tds;
   float salinity;
  
};

struct  Light {
    
  uint32_t lum, lux;
  uint16_t ir, full, vis;
  
};

#endif;
