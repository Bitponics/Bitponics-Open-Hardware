#define OUTLETNUM 2 //number of outlets

Cycle outlet[OUTLETNUM];
OutletStatus outletState[OUTLETNUM]; 
int relayPin[] = {25,26}; //our pins
boolean bCyclesStored = false;
unsigned long checkTime = 3600;
unsigned long checkLimit = 7400;

void setupRelayPins(){
  for(int i = 0; i<OUTLETNUM; i++){
    pinMode(relayPin[i], OUTPUT);
    //digitalWrite(relayPin[i],1);
  }
}

/* Operation to run Cycles */
boolean readCycles(char* _i){
  
  IncomingCycle split = parseIncoming(_i); 
  outlet[0] = parseCycle(split.one);
  printCycle(outlet[0], "Outlet 1");
  outlet[1] = parseCycle(split.two);
  printCycle(outlet[1], "Outlet 2");
  bCyclesStored = true;
  setupCycles();
  
}
void cycleLoop(){
  if(checkLimit<millis()){
    checkCycles();
    checkLimit = millis()+checkTime;
  }
}

void setupCycles(){
  Serial.println();
  Serial.println("Setting Up Cycles");
  
  for(int i = 0; i<OUTLETNUM; i++){
      if(outlet[i].override!=0){
        outletState[i].ID = outlet[i].ID;
        outletState[i].pin = relayPin[i];
        
        if(outlet[i].live.val == outlet[i].rest.val){
           //run infinitely 
           outletState[i].dur=0;
           outletState[i].val=outlet[i].live.val;
           
        }else{
           //check where we are in the current cycle
           unsigned long totalCycle = outlet[i].live.dur + outlet[i].rest.dur;
           if(outlet[i].offset > outlet[i].live.dur ){
             //then we are in the second cycle
             //unsigned long totalCycleDurLeft =  totalCycle - outlet[i].offset; //get the millis() left
             outletState[i].dur = totalCycle-(unsigned long)outlet[i].offset; //get the millis() left in our duration
             outletState[i].val=outlet[i].rest.val;
          }else{
            //then we are in the first cycle  
            outletState[i].dur = outlet[i].live.dur-outlet[i].offset;
            outletState[i].val=outlet[i].live.val; 
          }
  
        }//endif infinite check
        Serial.print("Writing Outlet: ");Serial.println(i);Serial.print(" Value: ");Serial.print(outletState[i].val);Serial.print(" Pin: ");Serial.println(outletState[i].pin);
        
        digitalWrite(outletState[i].pin, outletState[i].val);
        outletState[i].endT = outletState[i].dur + millis();
    }
  }
}

void changeCycle(int i){

  if(outletState[i].cur==0){
    outletState[i].dur = outlet[i].rest.dur;
    outletState[i].val = outlet[i].rest.val;
  }else{
    outletState[i].dur = outlet[i].live.dur;
    outletState[i].dur = outlet[i].live.val;
  }

  Serial.print("Writing Outlet: ");Serial.println(i);Serial.print(" Value: ");Serial.print(outletState[i].val);Serial.print(" Pin: ");Serial.println(outletState[i].pin);
  
  digitalWrite(outletState[i].pin, outletState[i].val);
  outletState[i].endT = outletState[i].dur + millis();

}

void checkCycles(){ 
  if(bCyclesStored){
    Serial.println();
    Serial.println("Checking Cycles");
    Serial.println();
    for(int i = 0; i< OUTLETNUM; i++){
      if(outletState[i].endT < millis() && outletState[i].dur != 0 ){ //if we are set to infinite 
         //make a change
         Serial.print("Over Time: "); Serial.println(outletState[i].pin); Serial.print("Current State: "); Serial.println(outletState[i].cur); Serial.print("Value: "); Serial.println(outletState[i].val); 
         changeCycle(i);// flip cycles
      }else{
         //digitalWrite(outletState[i].pin,outletState[i].val);
      }
    }
  }  
}

/* Parse out Incoming Cycles from Get CYCLE page */
IncomingCycle parseIncoming(String _inc){
    IncomingCycle c;
    int e = _inc.indexOf(';'); // find the ;
    c.one = _inc.substring(0,e+1); // get the string from beginning to ;
    c.two = _inc.substring(e+1); // get the string from ; to end
    return c;
}

/*custom cycle object stores current grow cycle*/
Cycle parseCycle(String _cycle){
    Cycle t;
    char arr[_cycle.length()]; //create buffer 
    _cycle.toCharArray(arr,sizeof(arr)); //put string in buffer 
    sscanf(arr,"%d,%d,%ld,%d,%ld,%d,%ld;", &t.ID, &t.override, &t.offset, &t.live.val, &t.live.dur, &t.rest.val, &t.rest.dur );//parse buffer
    return t;
}

//print cycle data to ensure success
void printCycle(Cycle c, String _name){
     Serial.print( _name +" ID: ");
     Serial.println(c.ID);
     Serial.print( _name + " Override: ");
     Serial.println(c.override);
     Serial.print(_name + " Offset: ");
     Serial.println(c.offset);
     Serial.print( _name +" Live Value: ");
     Serial.print(c.live.val);
     Serial.print("\t Duration: ");
     Serial.println(c.live.dur);
     Serial.print(_name + " Rest Value: ");
     Serial.print(c.rest.val);
     Serial.print("\t Duration: ");
     Serial.println(c.rest.dur);
}
