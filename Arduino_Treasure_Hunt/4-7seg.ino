#include "SevSeg.h"
SevSeg sevseg; 

const int analogPin = A2;//the analog input pin attach to
int inputValue = 0;//variable to store the value coming from sensor
int outputValue = 0;//variable to store the output value
const int buttonPin = 19; //the number of the key pin
int buttonVal = 1;

void setup(){
  byte numDigits = 4;
  byte digitPins[] = {10, 11, 12, 13};
  byte segmentPins[] = {9, 2, 3, 5, 6, 8, 7, 4};
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);
  bool resistorsOnSegments = true; 
  bool updateWithDelaysIn = true;
  byte hardwareConfig = COMMON_CATHODE; 
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(100);
  Serial.begin(9600);
}

void loop(){
  int value = readPot();
  if(digitalRead(buttonPin) == LOW){
      if (value == 12){
          for (int k=0; k < 60000; k++){
          sevseg.setChars("SHOE"); //shoe
          sevseg.refreshDisplay();
        }
      }
    else{
      for (int k=0; k < 32000; k++){
        sevseg.setChars("no");
        sevseg.refreshDisplay();
      }
    }
  }
}

int readPot (){
    inputValue = analogRead(analogPin);//read the value from the sensor
    Serial.println(inputValue);
    outputValue = map(inputValue,0,1023,0,20);//Convert from 0-1023 proportional to the number of a number of from 0 to 255
    sevseg.setNumber(outputValue);
    sevseg.refreshDisplay(); 
    return (outputValue);
  
}

