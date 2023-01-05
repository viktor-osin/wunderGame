#include <Wire.h>
#include <FastLED.h>
CRGB leds[92]; 

void setup() {
  FastLED.addLeds<NEOPIXEL,8>(leds, 92); 
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
}
void loop() 
{
  
}
// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
  if (x == 66 || x == 55)
  {
    //старт!
    for(int i = 0; i<15; i++){
      for(int j = 0; j < 46; j++){
        leds[int(j)] = CHSV(0,0,255);
      } 
      for(int j = 46; j < 92; j++){
        leds[int(j)] = CHSV(0,0,0);
      } 
      FastLED.show();
      delay(100);
      for(int j = 0; j < 46; j++){
        leds[int(j)] = CHSV(0,0,0);
      } 
      for(int j = 46; j < 92; j++){
        leds[int(j)] = CHSV(0,0,255);
      } 
      FastLED.show(); 
      delay(100);
    }
  }
  if (x == 11)
  {
    //зеленый
    for(int i = 0; i < 92; i++){
      leds[int(i)] = CHSV(90,255,255);
    } 
    FastLED.show(); 
  }
  if (x == 22)
  {
    //желтый
    for(int i = 0; i < 92; i++){
      leds[int(i)] = CHSV(42,255,255);
    }  
      FastLED.show(); 
  }
  if (x == 33)
  {
    //синий
    for(int i = 0; i < 92; i++){
      leds[int(i)] = CHSV(160,255,255);
    }  
      FastLED.show(); 
  }
  if (x == 44)
  {
    //красный
    for(int i = 0; i < 92; i++){
      leds[int(i)] = CHSV(0,255,255);
    }  
      FastLED.show(); 
  }
}
