#define BLUE 10
#define GREEN 11
#define RED 12

void setupLedPins(){
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT); 
}

const byte red[] = {
  255, 0,0};
const byte yellow[] = {
  255, 255,0};
const byte blue[] = {
  0,0,255};
const byte green[] = {
  0,0,255};
const byte purple[] = {
  255,0,255};


void setColor(const byte rgb[]){
  analogWrite(RED, rgb[0]);
  analogWrite(GREEN, rgb[1]);
  analogWrite(BLUE, rgb[2]);
  delay(2000);
}


