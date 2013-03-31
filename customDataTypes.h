//Custom Datatype for Bitponics project

#ifndef customDataTypes_H
#define customDataTypes_H

struct IncomingCycle{
  String one;
  String two;
};

struct Value {
  int val;
  long int dur; 
    
};

struct  Cycle{
  int ID;
  int override;
  long int offset;
  Value live; //OutletStatus.cur=0
  Value rest; //OutletStatus.cur=1
};

struct OutletStatus{
  int ID;
  int pin;
  int cur; // 0/1:live/rest
  unsigned long dur;
  unsigned long endT;
  int val;
};

struct EC{

   long int conductivity;
   long int tds;
   float salinity;
  
};


#endif;
