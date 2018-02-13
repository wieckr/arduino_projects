// color swirl! connect an RGB LED to the PWM pins as indicated
// in the #defines
// public domain, enjoy!
 
#define REDPIN 3
#define GREENPIN 5
#define BLUEPIN 6

#define FADESPEED 40     // make this higher to slow down

int status; 
int r, g, b;

void setup() {
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  Serial.begin(9600);
  
  //status = 0; //LED off
}
 
 
void loop() {
  
    if (Serial.available()){
    status = Serial.read() - '0';
  }
  
  if (status == 1) { //fading colors
   
  // fade from blue to violet
  for (r = 0; r < 256; r++) { 
    analogWrite(REDPIN, r);
    delay(FADESPEED);
  } 
  // fade from violet to red
  for (b = 255; b > 0; b--) { 
    analogWrite(BLUEPIN, b);
    delay(FADESPEED);
  } 
  // fade from red to yellow
  for (g = 0; g < 256; g++) { 
    analogWrite(GREENPIN, g);
    delay(FADESPEED);
  } 
  // fade from yellow to green
  for (r = 255; r > 0; r--) { 
    analogWrite(REDPIN, r);
    delay(FADESPEED);
  } 
  // fade from green to teal
  for (b = 0; b < 256; b++) { 
    analogWrite(BLUEPIN, b);
    delay(FADESPEED);
  } 
  // fade from teal to blue
  for (g = 255; g > 0; g--) { 
    analogWrite(GREENPIN, g);
    delay(FADESPEED);
  } 
}
  else if (status == 2) { //yellow breathe
    float val = (exp(sin(millis()/2000.0*PI)) - 0.36787944)*108.0;
    analogWrite(REDPIN, val);
    analogWrite(GREENPIN, val);
    analogWrite(BLUEPIN, 0);
  }
    else if (status == 3) { //red breathe
    float val = (exp(sin(millis()/500.0*PI)) - 0.36787944)*108.0;
    analogWrite(REDPIN, val);
    analogWrite(GREENPIN, 0);
    analogWrite(BLUEPIN, 0);
  }
    else if (status == 4) { //testing breathe
    float val = (exp(sin(millis()/500.0*PI)) - 0.36787944)*108.0;
    analogWrite(REDPIN, val);
    analogWrite(GREENPIN, 0);
    analogWrite(BLUEPIN, val);
   } 
  else { //led should be off
    analogWrite(GREENPIN,0);
    analogWrite(BLUEPIN, 0);
    analogWrite(REDPIN, 0);
  }
}
