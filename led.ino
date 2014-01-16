#define BLUE_PIN 10
#define GREEN_PIN 11
#define RED_PIN 12

void setupLED(){
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT); 
  setColor(RED);
}

void setColor(const byte rgb[]){
  analogWrite(RED_PIN, rgb[0]);
  analogWrite(GREEN_PIN, rgb[1]);
  analogWrite(BLUE_PIN, rgb[2]);
}



//void flash(color c, int d){
// setColor(c);
// delay(d);
// setColor(BLACK);
// delay(d)
// setColor(c);
// delay(d);
// setColor(BLACK);
//}
