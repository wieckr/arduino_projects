// Include the Servo library 
#include <Servo.h> 
// Declare the Servo pin 
int servoPin = 8; 
// Create a servo object 
Servo Servo1; 

int redPin = 7;
int whitePin = 6;
int bluePin = 2;
int redLED = 4;
int greenLED = 12;
int blueLED = 13;
int defDelay = 60;
int numPushed = 0;
int prevPush;
int inputSeq = 0;
int answers[3];


void setup() { 
   // We need to attach the servo to the used pin number 
   Servo1.attach(servoPin); 
   pinMode(redPin, INPUT);
   pinMode(whitePin, INPUT);
   pinMode(bluePin, INPUT);
   digitalWrite(bluePin, HIGH);
   digitalWrite(whitePin, HIGH);
   digitalWrite(redPin, HIGH);
   pinMode(redLED, OUTPUT);
   pinMode(greenLED, OUTPUT);
   pinMode(blueLED, OUTPUT);
   Serial.begin(9600);
   Servo1.write(100);
}
void loop(){ 

    showTutorial();

}

void showTutorial () {
    //red
    analogWrite(redLED, 256);
    for (int k=0; k < defDelay; k++){
        checkForButton();
    }
    analogWrite(redLED, 0);  
    for (int k=0; k < defDelay/4; k++){
        checkForButton();
    }

    //white
    analogWrite(redLED, 256);
    analogWrite(blueLED, 256);
    analogWrite(greenLED, 256);
    for (int k=0; k < defDelay; k++){
        checkForButton();
    }
    analogWrite(redLED, 0);
    analogWrite(blueLED, 0);
    analogWrite(greenLED, 0);
    for (int k=0; k < defDelay/4; k++){
        checkForButton();
    }

        
    //blue
    analogWrite(blueLED, 256);
    for (int k=0; k < defDelay; k++){
        checkForButton();
    }
    analogWrite(blueLED, 0);
    for (int k=0; k < defDelay*3; k++){
        checkForButton();
    }
    
}

void checkForButton() {
  //filter out any noise by setting a time buffer
   int button = whatsPushed();
  //Serial.print("numPushed: ");
  //Serial.println(numPushed);
  //Serial.print("inputSeq: ");
  //Serial.println(inputSeq);
  //Serial.print("answers: ");
  //Serial.print(answers[0]);
  //Serial.print(answers[1]);
  //Serial.println(answers[2]);
  delay(15);
  if (inputSeq == 0)
    return; //no buttons have been pushed
  while (inputSeq == 1){
  if (button == 1){
    answers[numPushed-1] = 1;
  }
  else if (button == 2){
    answers[numPushed-1] = 2;
  }
  else if (button == 3){
    answers[numPushed-1] = 3;
  }
  if (answers[2] != 0){
    if ((answers[0] == 1) && (answers[1] == 2) && (answers[2] == 3)){
      //correct
      analogWrite(greenLED, 256);
      analogWrite(blueLED, 0);
      analogWrite(redLED, 0);
      Servo1.write(20);
      delay(100000);
      
    }
    else {
      for (int k =0; k < 3; k++){
        answers[k] = 0;
      }
      inputSeq =0;
      numPushed = 0;
      analogWrite(redLED, 256);
      analogWrite(blueLED, 0);
      analogWrite(greenLED, 0);
      delay(200);
      analogWrite(redLED, 0);
      delay(200);
      analogWrite(redLED, 256);
      delay(200);
      analogWrite(redLED,0);
      delay(200);
      analogWrite(redLED,256);
      delay(200);
      analogWrite(redLED,0);
      delay(200);
      analogWrite(redLED, 256);
      delay(200);
      analogWrite(redLED, 0);
      delay(200);
      return;
    }
  }
  checkForButton();
  }

}

int whatsPushed() { //order should be red white blue
  if ((digitalRead(redPin) == LOW) && (digitalRead(whitePin)==HIGH) && (digitalRead(bluePin) == HIGH) && (prevPush != 1)){
    Serial.print('1');
    prevPush = 1;
    inputSeq = 1;
    numPushed++;
    while(digitalRead(redPin) == LOW){
      analogWrite(redLED, 256);
      analogWrite(blueLED, 0);
      analogWrite(greenLED, 0);
    }
    analogWrite(redLED, 0);
    analogWrite(blueLED, 0);
    analogWrite(greenLED, 0);
    return 1; //red pushed
  }
  else if((digitalRead(whitePin) == LOW) && (digitalRead(redPin) == HIGH) && (digitalRead(bluePin) == HIGH)){
    Serial.print('2');
    prevPush =2;
    inputSeq = 1;
    numPushed++;
    while(digitalRead(whitePin) == LOW){
      analogWrite(redLED, 256);
      analogWrite(blueLED, 256);
      analogWrite(greenLED, 256);
    }
    analogWrite(redLED, 0);
    analogWrite(blueLED, 0);
    analogWrite(greenLED, 0);
    return 2; //white pushed
  }
    else if((digitalRead(bluePin) == LOW) && (digitalRead(whitePin) == HIGH) && (digitalRead(redPin) == HIGH)){
    Serial.print('3');
    prevPush =3;
    inputSeq = 1;
    numPushed++;
    while(digitalRead(bluePin) == LOW){
      analogWrite(redLED, 0);
      analogWrite(blueLED, 256);
      analogWrite(greenLED, 0);
    }
    analogWrite(redLED, 0);
    analogWrite(blueLED, 0);
    analogWrite(greenLED, 0);
    return 3; //blue pushed
    }
  else{
    prevPush = 0;
    return 0;
  }
} 


