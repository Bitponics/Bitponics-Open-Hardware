void setup() {
  
  Serial.begin(115200);
  wifiSetup(9600);
  
}

void loop(){
  
  wifiLoop();
  
  
}
