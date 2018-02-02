/*
Author: Jonathan Whitten
Name: Trinket Example Projects
Description: Attempts to perform activities listed in presentation
Date: 06JUNE2016
*/

//Add any include files here
#include <Adafruit_NeoPixel.h>


#define MODE_TRANSITION 256
enum{
  MODE_CIRCLE = 0,
  MODE_CW_CIRCLE,
  MODE_COLORCIRCLE,
  MODE_CIRCLEGROW,
  MODE_SPIRAL,
  MODE_SPIRALMIXER,
  MODE_BIKE_TURNSIGNAL,
  MODE_COMPASS
};
//Setup the data pin for the NeoPixel ring
int datapin = 6;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, datapin, NEO_GRBW + NEO_KHZ800);

//Prototype functions
void setPixel(int index, uint32_t c);
void setStrand(uint32_t c);

//Global variables
uint8_t mode = MODE_CIRCLE;
uint32_t modecounter = 0;

uint8_t simpleindex = 0;
uint8_t simpleindex2 = 0;
uint8_t indices[3] = {0,0,0};
uint8_t growindex = 0;//for growing illusion
uint8_t growdir = 0; //normal

//Put all code to perform once here
void setup()
{
  //Setup hardware
  strip.begin();
  
  //Set the mode of the device
  mode = MODE_CIRCLE;
  modecounter = 0;
  
  simpleindex = 0;
  simpleindex2 = 0;
  growindex = 0;
  growdir = 0;
  
  //strip.setBrightness(20);
}

//Put all repeating code here
void loop()
{
  uint32_t c; //color memory space
  uint8_t data[16] = {
    0, 0, 0, 0,
    0, 0, 0, 1, 
    5, 10, 14, 21,
    31, 47, 63, 255
  };
  
  switch(mode){
    case MODE_CIRCLE:
      clear(); //clear old values
      setPixel(simpleindex, strip.Color(0,0,0,255));//white pixel goes around
      simpleindex = (simpleindex+1)%16;
      break;
    case MODE_CW_CIRCLE:
      clear();
      setPixel(simpleindex, strip.Color(0,0,0,255));
      if(simpleindex == 0){
        simpleindex = 15;
      }else{
        simpleindex -= 1;
      }
      break;
    case MODE_COLORCIRCLE:
      //create the color of interest
      c = Wheel(simpleindex);
      clear();
      setPixel(simpleindex%16, c);
      simpleindex += 1;
      break;
    case MODE_CIRCLEGROW:
      //create color of interest
      c = Wheel(simpleindex);
      clear();
      for(uint8_t i=0;i<growindex+1;i++){
        setPixelNoShow((i+simpleindex)%16, c);
      }
      strip.show();
      if((simpleindex%6) == 0){
        if(growdir == 0){//normal grow direction
          if(growindex == 15){
            growdir = 1;
          }else{
            growindex += 1;   
          }
          
        }//end growdir == 0
        else{
          if(growindex == 0){
            growdir = 0;
          }else{
            growindex -= 1;
          }
        }
      }
      simpleindex += 1;
      break;
    case MODE_SPIRAL:
      //set color array
      for(uint8_t i=0;i<16;i++){
        switch(simpleindex2){
          case 0:
            setPixelNoShow((i+simpleindex)%16, strip.Color(data[i], 0, 0, 0));
          break;
          case 1:
            setPixelNoShow((i+simpleindex)%16, strip.Color(0, data[i], 0, 0));
          break;
          case 2:
            setPixelNoShow((i+simpleindex)%16, strip.Color(0, 0, data[i], 0));
          break;
          case 3:
            setPixelNoShow((i+simpleindex)%16, strip.Color(0, 0, 0, data[i]));
          break;
        }
        
      }
      strip.show();
      simpleindex += 1;
      
      if((simpleindex)%64==0){
        simpleindex2 = (simpleindex2+1)%4;
      }
      break;
    case MODE_SPIRALMIXER:
        for(uint8_t i=0;i<16;i++){
          setPixelNoShow(i, 
            strip.Color(
              data[15 - ((indices[0]+i)%16)],
              data[15 - ((indices[1]+i)%16)],
              data[15 - ((indices[2]+i)%16)],
              0)
          );
        }//endfor
        strip.show();
        //Update indices
        simpleindex += 1;
        
        indices[0] += 1;
        if((simpleindex%3)==0){
          indices[1] += 1;
        }
        if((simpleindex%5)==0){
          indices[2] += 1;
        }
      break;
    case MODE_BIKE_TURNSIGNAL:
      //break signal, triple blink red
      setStrand(strip.Color(255, 0, 0, 0));
      delay(70);
      
      clear();
      strip.show();
      delay(70);
      
      setStrand(strip.Color(255, 0, 0, 0));
      delay(70);
      
      clear();
      strip.show();
      delay(70);
      
      setStrand(strip.Color(255, 0, 0, 0));
      strip.show();
      delay(1000);
      
      clear();
      strip.show();
      delay(1000);
      
      //Turn left
      indicate_left();
      indicate_left();
      indicate_left();
      
      //Turn right
      indicate_right();
      indicate_right();
      indicate_right();
      mode = mode+1;
    break;
    case MODE_COMPASS:
      clear();
      setPixelNoShow(simpleindex%16, strip.Color(255, 0, 0, 0));
      setPixelNoShow((simpleindex+4)%16, strip.Color(0, 255, 0, 0));
      setPixelNoShow((simpleindex+8)%16, strip.Color(0, 0, 255, 0));
      setPixelNoShow((simpleindex+12)%16, strip.Color(0, 0, 0, 255));
      strip.show();
      
      simpleindex = simpleindex+1;
      
    break;
  }
  
  modecounter += 1;
  if(modecounter == MODE_TRANSITION){
    modecounter = 0;
    mode = (mode+1)%8;
  }
  delay(50); //update rate
}


void clear(){
  for(uint8_t i=0;i<strip.numPixels();i++){
    strip.setPixelColor(i, 0);
  } 
}

//Set a single pixel to a color
void setPixel(int index, uint32_t c){
  strip.setPixelColor(index, c);
  strip.show();
}

void setPixelNoShow(int index, uint32_t c){
  strip.setPixelColor(index, c);
}

//Strand helper function
/*
  Title: setStrand
  Changes the color of the entire string to the input color
  
  input: 
  uint32_t c  - color generated using strip.Color(r,g,b,w)
  
  output: void
*/
void setStrand(uint32_t c){
  for(uint8_t i=0;i<strip.numPixels();i++){
    strip.setPixelColor(i, c);
  }
  strip.show();
}

void indicate_left(){
  clear();
  for(uint8_t i=0;i<8;i++){
    strip.setPixelColor(i, strip.Color(200, 50, 0));
  }
  strip.show();
  delay(175);
  
  clear();
  strip.show();
  delay(175);
}

void indicate_right(){
  clear();
  for(uint8_t i=8;i<16;i++){
    strip.setPixelColor(i, strip.Color(200, 50, 0));
  }
  strip.show();
  delay(175);
  
  clear();
  strip.show();
  delay(175);
}

uint32_t Wheel(uint8_t WheelPos)
{
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
  WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
  WheelPos -= 170; 
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
