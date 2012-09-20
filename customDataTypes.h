//Custom Datatype for Bitponics project

//Arduino Struct Hack 
//http://arduino.cc/playground/Code/Struct#FAQ


struct Value{
  int val;
  long int dur; 
    
};

struct Cycle {
  int ID;
  int start;
  Value live;
  Value rest;
};

struct IncomingCycle{
  String one;
  String two;
};

