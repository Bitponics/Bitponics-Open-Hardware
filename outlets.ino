const int relayPin[] = {
  27,28,25,26,};

int curRelayState[] = {
  0,0}; 

void setupRelays(){  
  for(int i=0;i <4; i++){
    pinMode(relayPin[i], OUTPUT);
  }
}

void switchRelay(int relay, int state){
  digitalWrite(relayPin[relay*2+state],HIGH);
  delay(20);
  digitalWrite(relayPin[relay*2+state],LOW);
  Serial.print(F("   > relay "));
  Serial.print(relay);
  Serial.print(F(": "));
  Serial.println(state);
  curRelayState[relay] = state;

}

