#include <Arduino.h>
#include <TM1637Display.h>
#include <Time.h>
#include <TimeLib.h>
#include <virtuabotixRTC.h>
#include <SoftPWM.h>
#include <Keypad.h>
#include <avr/wdt.h>

#define CLK 30
#define DIO 28

//Connection Pins
int bombClock = 3600; //(20 * 60) seconds
int toggle_switch_pin = 56;
int toggle_led_pin = 12;//change 
int cut_the_wire_led_pin = 13;//change
int cut_the_wire_break_pin = 55; //changed was 32
int reed_start_switch_pin = 9; //change
int green_button_led_pin = 37;
int green_button_pin = 41;
int red_button_led_pin = 39;
int red_button_pin = 43;
int keypad_green_led_pin = 27; //changed was 29
int keypad_red_led_pin = 23; //changed was 5
int keypad_yellow_led_pin = 25; //changed was 6 //25 no working
int rgb_blue_pin = 62;
int rgb_green_pin = 63;
int rgb_red_pin = 64;
//int rbg_green_button_pin = 41;
//int rbg_blue_button_pin = 43;
//int rbg_yellow_button_pin = 1;  //change 
int membrane_1 = 2; //toe kick
int membrane_2 = 3; //swimdeck
int membrane_3 = 4; //bulwark
int membrane_4 = 5;//passe
int membrane_5 = 6; //swim deck edge
int membrane_6 = 7;//underwater
int membrane_led1 = 57;          //change         //change
int membrane_green_led = 10;

int rbg_blue_button_pin = 45;
int rbg_white_button_pin = 47;
int rbg_green_button_pin = 49;
int rbg_yellow_button_pin = 51;
int rbg_black_button_pin = 69; //was 53

int connect_wire1 = 60;
int connect_wire2 = 61;
int connect_led_pin = 11 ;


tmElements_t currentT;

//keypad
const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
//byte rowPins[ROWS] = {9, 8, 7, 6}; 
//byte colPins[COLS] = {5, 4, 3, 2};

//byte rowPins[ROWS] = {36, 38, 40, 42}; 
//byte colPins[COLS] = {44, 46, 48, 50};
byte rowPins[ROWS] = {50, 48, 46, 44}; 
byte colPins[COLS] = {42, 40, 38, 36};
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
Keypad keypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS );

//keypad code
char keyCode[10]= {}; //hard coded up to [7] for positive entries, don't make less than that
byte keyCodeIndex =0;

char membraneCode[6] = {};
byte membraneCodeIndex = 0;


byte leds = 0;
//RTC
//virtuabotixRTC myRTC(2, 3, 4); //If you change the wiring change the pins here also
virtuabotixRTC myRTC(26, 24, 22);
tmElements_t startT;
time_t startTSec;
//7-seg display
// Create display object of type TM1637Display:
TM1637Display display = TM1637Display(CLK, DIO);
// Create array that turns all segments on:
const uint8_t data[] = {0xff, 0xff, 0xff, 0xff};
// Create array that turns all segments off:
const uint8_t blank[] = {0x00, 0x00, 0x00, 0x00};

//Custom variables
int started = 0; //clock started 
int prevBombTime = 0;
int badEntered = 0; //if BAD code entered, can enable RGB light
int membraneStarted = 0;
int oldMembranePress = 7;

//finished Variables
int membraneFinished = 0; //
int toggleFinished = 0;  //
int cutWireFinished = 0; //
int rgbFinished = 0;
int keypadFinished  = 0; //
int connectWireFinished = 0; //
int bigButtonFinished = 0; //

int allFinished = 0; //
int finishTime = 0;

void setup() {
  //For reset
  MCUSR = 0;
  delay(10000);
  pinMode(toggle_switch_pin, INPUT_PULLUP);
  pinMode(toggle_led_pin, OUTPUT);
  pinMode(cut_the_wire_led_pin, OUTPUT);
  pinMode(cut_the_wire_break_pin, INPUT_PULLUP);
  pinMode(keypad_green_led_pin, OUTPUT);
  pinMode(green_button_pin, INPUT_PULLUP);
  pinMode(red_button_pin, INPUT_PULLUP);
  pinMode(rbg_blue_button_pin, INPUT_PULLUP);
  pinMode(rbg_white_button_pin, INPUT_PULLUP);
  pinMode(rbg_green_button_pin, INPUT_PULLUP);
  pinMode(rbg_yellow_button_pin, INPUT_PULLUP);
  pinMode(rbg_black_button_pin, INPUT_PULLUP);
  //pinMode(membrane_led1, OUTPUT);
  pinMode(membrane_1, INPUT_PULLUP);
  pinMode(membrane_2, INPUT_PULLUP);
  pinMode(membrane_3, INPUT_PULLUP);
  pinMode(membrane_4, INPUT_PULLUP);
  pinMode(membrane_5, INPUT_PULLUP);
  pinMode(membrane_6, INPUT_PULLUP);
  pinMode(membrane_green_led, OUTPUT);
  pinMode(connect_wire1, INPUT_PULLUP);
  pinMode(connect_wire2, INPUT_PULLUP);
  pinMode(connect_led_pin, OUTPUT);
  pinMode(reed_start_switch_pin, INPUT_PULLUP);
  
 //4x7segment display 
  display.clear();
  display.setBrightness(6);
  
  Serial.begin(9600);

  SoftPWMBegin();
  SoftPWMSet(membrane_led1, 0);
  SoftPWMSetFadeTime(membrane_led1, 1000, 1);
  SoftPWMSet(green_button_led_pin, 0);
  SoftPWMSetFadeTime(green_button_led_pin, 1000, 1000);
  SoftPWMSet(red_button_led_pin, 0);
  SoftPWMSetFadeTime(red_button_led_pin, 1000, 1000);
  SoftPWMSet(keypad_yellow_led_pin, 0);
  SoftPWMSetFadeTime(keypad_yellow_led_pin, 2, 50);
  SoftPWMSet(keypad_red_led_pin, 0);
  SoftPWMSetFadeTime(keypad_red_led_pin, 5, 375);
  SoftPWMSet(rgb_blue_pin, 0);
  SoftPWMSetFadeTime(rgb_blue_pin, 5, 375);
  SoftPWMSet(rgb_red_pin, 0);
  SoftPWMSetFadeTime(rgb_red_pin, 5, 375);
  SoftPWMSet(rgb_green_pin, 0);
  SoftPWMSetFadeTime(rgb_green_pin, 5, 375);
}


void loop() {
   
  toggle_module();
  cut_the_wire_module();
  rgb_module();
  connect_module();
  membrane_button_module();
  big_button_module();
  finish_module(finishTime);

  /////////////////////Interupts //////////////////////////


  
  char keypress = get_keypress();
  if (keypress){
    enter_code(keypress);
  }

  

  //  red and green buttons
  int greenButtonPress = get_green_button_press();
  if (greenButtonPress){
   SoftPWMSetPercent(green_button_led_pin, 100);
  }
  else if (bigButtonFinished == 0){
   SoftPWMSetPercent(green_button_led_pin, 0);
  }
  else if (bigButtonFinished ==1){
    SoftPWMSetPercent(green_button_led_pin, 50);
  }

  int redButtonPress = get_red_button_press();
  if (redButtonPress){
   SoftPWMSetPercent(red_button_led_pin, 100);
  }
  else{
   SoftPWMSetPercent(red_button_led_pin, 0);
  }
  
  //membrane switches
  int newMembranePress = get_membrane_press();
  if (newMembranePress != oldMembranePress && newMembranePress && membraneFinished == 0){
    oldMembranePress = newMembranePress;
    digitalWrite(membrane_led1, LOW);
    if (membraneStarted){
      enter_membrane_code(newMembranePress);
    }
  }

  
  //Timer
  int bombTime = get_time();
  if (allFinished == 0){
    display_time(bombTime);
  }
  else{
    Serial.println("all completed, show code");
    allFinished =1;
    
  }
  if (digitalRead(reed_start_switch_pin) == LOW && started == 0){
    display_startup();
    set_start_time();
    started = 1;
  }
}

void toggle_module(){
    //for on/off toggle switch, hardware changes the position
  if(digitalRead(toggle_switch_pin) == HIGH){
    digitalWrite(toggle_led_pin, LOW);
    toggleFinished = 0;
  }
  if(digitalRead(toggle_switch_pin) == LOW && get_green_button_press()){
  //if(digitalRead(toggle_switch_pin) == LOW){
    digitalWrite(toggle_led_pin, HIGH);
    Serial.println("toggle correct");
    toggleFinished = 1;
    
  }
}

void cut_the_wire_module(){
  //one side is hot, the other is cold, they have to cut all wires
  if(digitalRead(cut_the_wire_break_pin) == HIGH){
    Serial.println("cut the wire--cut");
    digitalWrite(cut_the_wire_led_pin, HIGH);
    cutWireFinished = 1;
    
  }
  if(digitalRead(cut_the_wire_break_pin) == LOW){
    digitalWrite(cut_the_wire_led_pin, LOW);
  }
}

void set_start_time(){
  //RTC 
  myRTC.updateTime();  
  startT.Hour = myRTC.hours;
  startT.Minute = myRTC.minutes;
  startT.Second = myRTC.seconds;
  startT.Day = myRTC.dayofmonth;
  startT.Month = myRTC.month;
  startT.Year = myRTC.year - 1970;
  startTSec = makeTime( startT );

}

int get_time(){
  //Gets time since "start" from RTC and returns minutes and seconds as 4 digit string, i.e. MMSS
  myRTC.updateTime();
  //tmElements_t currentT;
 
  currentT.Hour = myRTC.hours;
  currentT.Minute = myRTC.minutes;
  currentT.Second = myRTC.seconds;
  currentT.Day = myRTC.dayofmonth;
  currentT.Month = myRTC.month;
  currentT.Year = myRTC.year - 1970;

  time_t currentTSec = makeTime( currentT);
  int32_t diff = startTSec - currentTSec;
  int tempBombClock = bombClock + diff;
  int bombMin = (tempBombClock / 60);
  int bombSec = (tempBombClock - (bombMin * 60)) % 60; 
  return (bombMin*100 + bombSec);
  
}
void display_time(int bombTime){
  if (started == 0){
    display.setSegments(blank);
  }
  else if (bombTime > -1){
     if (prevBombTime != bombTime){ 
       display.showNumberDecEx(bombTime, 0b11100000, false, 4, 0);
     }
  }
  else if (bombTime >0){
    display.showNumberDecEx(0000, 0b11100000, false, 4, 0);
  }
}

void display_startup(){
  Serial.println("started clock");
  delay(2000);
  for (int i = 0; i<6000; i+=40){
    display.showNumberDecEx(i, 0b11100000, false, 4, 0);
    delay(1);
  }
}

char get_keypress(){
  char key = keypad.getKey();
  if (key){
    Serial.print("key");
    Serial.println(key);
    return(key);
  }
  else
    return (false);
}
void pause_clock(){
  //TODO
  
}
void number_code_bad(){
  //flash red light, or something
  //TODO change to pwm
  //digitalWrite(keypad_red_led_pin, HIGH);
  SoftPWMSetPercent(keypad_red_led_pin, 100);
  delay(200);
  SoftPWMSetPercent(keypad_red_led_pin, 0);
  delay(200);
  SoftPWMSetPercent(keypad_red_led_pin, 100);
  delay(200);
  SoftPWMSetPercent(keypad_red_led_pin, 0);
  delay(200);
  SoftPWMSetPercent(keypad_red_led_pin, 100);
  delay(200);
  SoftPWMSetPercent(keypad_red_led_pin, 0);
  delay(200);
  SoftPWMSetPercent(keypad_red_led_pin, 100);
  delay(200);
  SoftPWMSetPercent(keypad_red_led_pin, 0);
  
}

void number_entered(){
  SoftPWMSetPercent(keypad_yellow_led_pin, 100);
  delay(200);
  SoftPWMSetPercent(keypad_yellow_led_pin, 0);
}

int enter_code(char key){
  if (key == '#'){
    number_entered();
    check_key_pattern(key);
  }
  else{
    if (keyCodeIndex == sizeof(keyCode)){//too many numbers entered, clear out array, send no dice signal
      memset(keyCode, 0, sizeof(keyCode));
      keyCodeIndex = 0;
      Serial.println("too many numbers entered");
      number_code_bad();
    }
    else{
      keyCode[keyCodeIndex] = key;
      keyCodeIndex++;
      number_entered();
    }
  }
  return(0);
}

int check_key_pattern(char key){
  if (keyCode[0] == '7' && keyCode[1] == '9' && keyCode[2] == '3' && keyCode[3] == '5' && keyCode[4] == 0 && keyCode[5] == 0 && keyCode[6] == 0 && keyCode[7] == 0){
    //7935 code entered correctly
    Serial.println("keypad, completed");
    digitalWrite(keypad_green_led_pin, HIGH);
    keypadFinished = 1;
  }
  else if (keyCode[0] == '*' && keyCode[1] == '7' && keyCode[2] == '8' && keyCode[3] == '8' && keyCode[4] == '7' && keyCode[5] == '*' && keyCode[6] == 0 && keyCode[7] == 0 && keyCode[8] == 0){
    //*7887 pause or resume clock
    pause_clock();
    Serial.println("clock paused");
  }
  else if (keyCode[0] == '*' && keyCode[1] == '8' && keyCode[2] == '7' && keyCode[3] == '7' && keyCode[4] == '8' && keyCode[5] == '*' && keyCode[6] == 0 && keyCode[7] == 0 && keyCode[8] == 0){
    //*8778 reset clock
    set_start_time();
    Serial.println("clock reset");
  }
  else if (keyCode[0] == '*' && keyCode[1] == '1' && keyCode[2] == '0' && keyCode[3] == '1' && keyCode[4] == '0' && keyCode[5] == '*' && keyCode[6] == 0 && keyCode[7] == 0 && keyCode[8] == 0){
    //*1010 add 10 minutes
    Serial.println("add 10 minutes");
    bombClock += 600;
  }
  else if (keyCode[0] == '*' && keyCode[1] == '9' && keyCode[2] == '9' && keyCode[3] == '9' && keyCode[4] == '9' && keyCode[5] == '*' && keyCode[6] == 0 && keyCode[7] == 0 && keyCode[8] == 0){
    wdt_enable(WDTO_15MS); // turn on the WatchDog and don't stroke it.
    for(;;) { 
     // do nothing and wait for the eventual...
    } 
  }
  else if (keyCode[0] == 'B' && keyCode[1] == 'A' && keyCode[2] == 'D' && keyCode[3] == 0 && keyCode[4] == 0 && keyCode[5] == 0 && keyCode[6] == 0 && keyCode[7] == 0){
    //BAD code entered correctly
    Serial.println("correct bad keypad, completed");
    badEntered = 1;
    
  }
  else{
    Serial.println("bad code entered");
    number_code_bad();
   }
   memset(keyCode, 0, sizeof(keyCode));
   keyCodeIndex = 0;
}

void enter_membrane_code(int button){
   SoftPWMSetPercent(membrane_led1, 0);
   Serial.println(button);
   membraneCode[membraneCodeIndex] = button;
   membraneCodeIndex++;
   if (membraneCodeIndex == 6){
    check_membrane_pattern();
   }

}

void check_membrane_pattern(){
  if (membraneCode[0] == 4 && membraneCode[1] == 6 && membraneCode[2] == 1 && membraneCode[3] == 5 && membraneCode[4] == 2 && membraneCode[5] == 3){
    digitalWrite(membrane_green_led, HIGH);
    Serial.println("correct membrane");
    digitalWrite(membrane_led1, LOW);
    membraneFinished = 1;
  }
  else{
   //reset
   Serial.println("membrane_code_reset");
  SoftPWMSetPercent(red_button_led_pin, 100);
  delay(200);
  SoftPWMSetPercent(red_button_led_pin, 0);
  delay(200);
  SoftPWMSetPercent(red_button_led_pin, 100);
  delay(200);
  SoftPWMSetPercent(red_button_led_pin, 0);
  delay(200);
  SoftPWMSetPercent(red_button_led_pin, 100);
  delay(200);
  SoftPWMSetPercent(red_button_led_pin, 0);
  delay(200);
  SoftPWMSetPercent(red_button_led_pin, 100);
  delay(200);
  SoftPWMSetPercent(red_button_led_pin, 0);
   memset(membraneCode, 0, sizeof(membraneCode));
   membraneCodeIndex = 0;
   oldMembranePress = 7;
  }
}


int get_green_button_press(){
  if(digitalRead(green_button_pin) == HIGH){
    return(0);
  }
  else{
    return(1);
  }
}
int get_red_button_press(){
  if(digitalRead(red_button_pin) == HIGH){
    return(0);
  }
  else{
    return(1);
  }
}

//void rgb_module(){ //mix colors to make.....green? purple? orange?
//  if (badEntered){
//    if (digitalRead(rbg_blue_button_pin) == LOW && digitalRead(rbg_white_button_pin) == HIGH && digitalRead(rbg_green_button_pin) == HIGH && digitalRead(rbg_black_button_pin) == HIGH && digitalRead(rbg_yellow_button_pin) == HIGH){//Doesn't work yet
//      Serial.println("blue button pushed");
//      SoftPWMSetPercent(rgb_blue_pin, 100);
//    }
//    else if (digitalRead(rbg_blue_button_pin) == HIGH && digitalRead(rbg_white_button_pin) == LOW && digitalRead(rbg_green_button_pin) == HIGH && digitalRead(rbg_black_button_pin) == HIGH && digitalRead(rbg_yellow_button_pin) == HIGH){//Doesn't work yet
//        Serial.println("white button pushed");
//        SoftPWMSetPercent(rgb_red_pin, 100);
//        SoftPWMSetPercent(rgb_blue_pin, 100);
//        SoftPWMSetPercent(rgb_green_pin, 100);
//    }
//    else if (digitalRead(rbg_blue_button_pin) == HIGH && digitalRead(rbg_white_button_pin) == HIGH && digitalRead(rbg_green_button_pin) == LOW && digitalRead(rbg_black_button_pin) == HIGH && digitalRead(rbg_yellow_button_pin) == HIGH){//Doesn't work yet
//        Serial.println("green button pushed");
//        SoftPWMSetPercent(rgb_green_pin, 100);
//    }
//    else if (digitalRead(rbg_blue_button_pin) == HIGH && digitalRead(rbg_white_button_pin) == HIGH && digitalRead(rbg_green_button_pin) == HIGH && digitalRead(rbg_black_button_pin) == LOW && digitalRead(rbg_yellow_button_pin) == HIGH){//Doesn't work yet
//        Serial.println("black button pushed");
//        SoftPWMSetPercent(rgb_red_pin, 100);
//        SoftPWMSetPercent(rgb_green_pin, 0);
//        SoftPWMSetPercent(rgb_blue_pin, 0);
//    }
//    else if (digitalRead(rbg_blue_button_pin) == HIGH && digitalRead(rbg_white_button_pin) == HIGH && digitalRead(rbg_green_button_pin) == HIGH && digitalRead(rbg_black_button_pin) == HIGH && digitalRead(rbg_yellow_button_pin) == LOW){//Doesn't work yet
//        Serial.println("yellow button pushed");
//        SoftPWMSetPercent(rgb_red_pin, 100);
//        SoftPWMSetPercent(rgb_green_pin, 100);
//        SoftPWMSetPercent(rgb_blue_pin, 0);
//    }
//    else{
//      SoftPWMSetPercent(rgb_red_pin, 0);
//      SoftPWMSetPercent(rgb_green_pin, 0);
//      SoftPWMSetPercent(rgb_blue_pin, 0);
//    }
//  }
//}

void rgb_module(){ //mix colors to make.....green? purple? orange?
  if (badEntered){
    if (digitalRead(rbg_blue_button_pin) == LOW && digitalRead(rbg_white_button_pin) == HIGH && digitalRead(rbg_green_button_pin) == HIGH && digitalRead(rbg_black_button_pin) == HIGH && digitalRead(rbg_yellow_button_pin) == HIGH && rgbFinished ==0){//Doesn't work yet
      Serial.println("blue button pushed, red light");
        SoftPWMSetPercent(rgb_red_pin, 80);
        SoftPWMSetPercent(rgb_blue_pin, 0);
        SoftPWMSetPercent(rgb_green_pin, 0);
    }
    else if (digitalRead(rbg_blue_button_pin) == HIGH && digitalRead(rbg_white_button_pin) == LOW && digitalRead(rbg_green_button_pin) == HIGH && digitalRead(rbg_black_button_pin) == HIGH && digitalRead(rbg_yellow_button_pin) == HIGH && rgbFinished ==0){//Doesn't work yet
        Serial.println("white button pushed, blue light");
        SoftPWMSetPercent(rgb_red_pin, 0);
        SoftPWMSetPercent(rgb_blue_pin, 80);
        SoftPWMSetPercent(rgb_green_pin, 0);
    }
    else if (digitalRead(rbg_blue_button_pin) == HIGH && digitalRead(rbg_white_button_pin) == HIGH && digitalRead(rbg_green_button_pin) == LOW && digitalRead(rbg_black_button_pin) == HIGH && digitalRead(rbg_yellow_button_pin) == HIGH && rgbFinished ==0){//Doesn't work yet
        Serial.println("green button pushed, fuchia light showing");
        SoftPWMSetPercent(rgb_red_pin, 80);
        SoftPWMSetPercent(rgb_blue_pin, 80);
        SoftPWMSetPercent(rgb_green_pin, 0);
    }
    else if (digitalRead(rbg_blue_button_pin) == HIGH && digitalRead(rbg_white_button_pin) == HIGH && digitalRead(rbg_green_button_pin) == HIGH && digitalRead(rbg_black_button_pin) == LOW && digitalRead(rbg_yellow_button_pin) == HIGH && rgbFinished ==0){//Doesn't work yet
        Serial.println("black button pushed, yellow light");
        SoftPWMSetPercent(rgb_red_pin, 80);
        SoftPWMSetPercent(rgb_green_pin, 80);
        SoftPWMSetPercent(rgb_blue_pin, 0);
    }
    else if (digitalRead(rbg_blue_button_pin) == HIGH && digitalRead(rbg_white_button_pin) == HIGH && digitalRead(rbg_green_button_pin) == HIGH && digitalRead(rbg_black_button_pin) == HIGH && digitalRead(rbg_yellow_button_pin) == LOW && rgbFinished ==0){//Doesn't work yet
        Serial.println("yellow button pushed, white light");
        SoftPWMSetPercent(rgb_red_pin, 80);
        SoftPWMSetPercent(rgb_green_pin, 80);
        SoftPWMSetPercent(rgb_blue_pin, 80);
    }
    else if (digitalRead(rbg_blue_button_pin) == LOW && digitalRead(rbg_white_button_pin) == LOW && digitalRead(rbg_green_button_pin) == HIGH && digitalRead(rbg_black_button_pin) == HIGH && digitalRead(rbg_yellow_button_pin) == HIGH && rgbFinished ==0){//Doesn't work yet
      Serial.println("blue and white pushed, fuchia light");
        SoftPWMSetPercent(rgb_red_pin, 80);
        SoftPWMSetPercent(rgb_blue_pin, 80);
        SoftPWMSetPercent(rgb_green_pin, 0);
    }
    else if (digitalRead(rbg_blue_button_pin) == LOW && digitalRead(rbg_white_button_pin) == HIGH && digitalRead(rbg_green_button_pin) == LOW && digitalRead(rbg_black_button_pin) == HIGH && digitalRead(rbg_yellow_button_pin) == HIGH && rgbFinished ==0){//Doesn't work yet
        Serial.println(" blue and green button pushed, purple light");
        SoftPWMSetPercent(rgb_red_pin, 100);
        SoftPWMSetPercent(rgb_blue_pin, 100);
        SoftPWMSetPercent(rgb_green_pin, 0);
    }
    else if (digitalRead(rbg_blue_button_pin) == LOW && digitalRead(rbg_white_button_pin) == HIGH && digitalRead(rbg_green_button_pin) == HIGH && digitalRead(rbg_black_button_pin) == HIGH && digitalRead(rbg_yellow_button_pin) == LOW && rgbFinished ==0){//Doesn't work yet
        Serial.println("blue and yellow button pushed, fuchia light showing");
        SoftPWMSetPercent(rgb_red_pin, 100);
        SoftPWMSetPercent(rgb_blue_pin, 40);
        SoftPWMSetPercent(rgb_green_pin, 40);
    }
    else if (digitalRead(rbg_blue_button_pin) == LOW && digitalRead(rbg_white_button_pin) == HIGH && digitalRead(rbg_green_button_pin) == HIGH && digitalRead(rbg_black_button_pin) == LOW && digitalRead(rbg_yellow_button_pin) == HIGH && rgbFinished ==0){//Doesn't work yet
        Serial.println("blue and black button pushed, orange light"); //check later
        SoftPWMSetPercent(rgb_red_pin, 100);
        SoftPWMSetPercent(rgb_green_pin, 35);
        SoftPWMSetPercent(rgb_blue_pin, 0);
    }
    else if (digitalRead(rbg_blue_button_pin) == HIGH && digitalRead(rbg_white_button_pin) == LOW && digitalRead(rbg_green_button_pin) == LOW && digitalRead(rbg_black_button_pin) == HIGH && digitalRead(rbg_yellow_button_pin) == HIGH && rgbFinished ==0){//Doesn't work yet
        Serial.println("white and green button pushed, blue/fuchia light");
        SoftPWMSetPercent(rgb_red_pin, 40);
        SoftPWMSetPercent(rgb_green_pin, 0);
        SoftPWMSetPercent(rgb_blue_pin, 80);
    }
    else if (digitalRead(rbg_blue_button_pin) == HIGH && digitalRead(rbg_white_button_pin) == LOW && digitalRead(rbg_green_button_pin) == HIGH && digitalRead(rbg_black_button_pin) == HIGH && digitalRead(rbg_yellow_button_pin) == LOW && rgbFinished ==0){//Doesn't work yet
      Serial.println("white and yellow button pushed, blue/white light");
        SoftPWMSetPercent(rgb_red_pin, 60);
        SoftPWMSetPercent(rgb_blue_pin, 100);
        SoftPWMSetPercent(rgb_green_pin, 60);
    }
    else if (digitalRead(rbg_blue_button_pin) == HIGH && digitalRead(rbg_white_button_pin) == LOW && digitalRead(rbg_green_button_pin) == HIGH && digitalRead(rbg_black_button_pin) == LOW && digitalRead(rbg_yellow_button_pin) == HIGH && rgbFinished ==0){//Doesn't work yet
        Serial.println("white and black button pushed, green light");
        SoftPWMSetPercent(rgb_red_pin, 0);
        SoftPWMSetPercent(rgb_blue_pin, 0);
        SoftPWMSetPercent(rgb_green_pin, 80);
    }
    else if (digitalRead(rbg_blue_button_pin) == HIGH && digitalRead(rbg_white_button_pin) == HIGH && digitalRead(rbg_green_button_pin) == LOW && digitalRead(rbg_black_button_pin) == HIGH && digitalRead(rbg_yellow_button_pin) == LOW && rgbFinished ==0){//Doesn't work yet
        Serial.println(" green and yellow button pushed, fuchia  and white light showing");
        SoftPWMSetPercent(rgb_red_pin, 80);
        SoftPWMSetPercent(rgb_blue_pin, 80);
        SoftPWMSetPercent(rgb_green_pin, 20);
    }
    else if (digitalRead(rbg_blue_button_pin) == HIGH && digitalRead(rbg_white_button_pin) == HIGH && digitalRead(rbg_green_button_pin) == LOW && digitalRead(rbg_black_button_pin) == LOW && digitalRead(rbg_yellow_button_pin) == HIGH && rgbFinished ==0){//Doesn't work yet
        Serial.println("green and black button pushed, orange light"); //copy above
        SoftPWMSetPercent(rgb_red_pin, 100);
        SoftPWMSetPercent(rgb_green_pin, 35);
        SoftPWMSetPercent(rgb_blue_pin, 0);
    }
    else if (digitalRead(rbg_blue_button_pin) == HIGH && digitalRead(rbg_white_button_pin) == HIGH && digitalRead(rbg_green_button_pin) == HIGH && digitalRead(rbg_black_button_pin) == LOW && digitalRead(rbg_yellow_button_pin) == LOW && rgbFinished ==0){//Doesn't work yet
        Serial.println("yellow and black button pushed, white and yellow light");
        SoftPWMSetPercent(rgb_red_pin, 80);
        SoftPWMSetPercent(rgb_green_pin, 80);
        SoftPWMSetPercent(rgb_blue_pin, 20);
    }
    else if (digitalRead(rbg_blue_button_pin) == HIGH && digitalRead(rbg_white_button_pin) == LOW && digitalRead(rbg_green_button_pin) == LOW && digitalRead(rbg_black_button_pin) == LOW && digitalRead(rbg_yellow_button_pin) == HIGH){//Doesn't work yet
        Serial.println("correct rgb buttons");
        SoftPWMSetPercent(rgb_red_pin, 0);
        SoftPWMSetPercent(rgb_green_pin, 10);
        SoftPWMSetPercent(rgb_blue_pin, 0);
        rgbFinished = 1;
    }
    else if (rgbFinished == 0){
      SoftPWMSetPercent(rgb_red_pin, 0);
      SoftPWMSetPercent(rgb_green_pin, 0);
      SoftPWMSetPercent(rgb_blue_pin, 0);
    }
    else if (rgbFinished == 1){
        SoftPWMSetPercent(rgb_red_pin, 0);
        SoftPWMSetPercent(rgb_green_pin, 10);
        SoftPWMSetPercent(rgb_blue_pin, 0);
    }
  }
}
void membrane_button_module(){
  if (get_green_button_press() && get_red_button_press() && get_toe_kick_press()){
    membraneStarted = 1;
    SoftPWMSetPercent(membrane_led1, 100);
    Serial.println("membrane led lit");
    delay(3000);
  }
  if (membraneStarted){
    SoftPWMSetPercent(membrane_led1, 100);
  }
}

int get_membrane_press(){
  if( digitalRead(membrane_1) == LOW){
    Serial.println("toe_kick pressed");
     return(1);
  }
  if( digitalRead(membrane_3) == LOW){
    Serial.println("bulwark_grazing pressed");
     return(2);
  }
  if( digitalRead(membrane_2) == LOW){
    Serial.println("swim deck handrail pressed");
     return(3);
  }
  if( digitalRead(membrane_5) == LOW){
    Serial.println("swim deck edge pressed");
     return(4);
  }
  if( digitalRead(membrane_4) == LOW){
    Serial.println("passerelle pressed");
     return(5);
  }
  if( digitalRead(membrane_6) == LOW){
    Serial.println("underwater pressed");
     return(6);
  }
  else {
    return (0);
  }
}
//
int get_toe_kick_press(){
  if( digitalRead(membrane_1) == LOW){
    Serial.println("toe_kick pressed");
     return(1);
  }
  else{
    return (0);
  }
}


void connect_module(){
  if (digitalRead(connect_wire1) == LOW && digitalRead(connect_wire2) == LOW && get_red_button_press()){
    Serial.println("connnected wires successfully");
    digitalWrite(connect_led_pin, HIGH);
    connectWireFinished = 1;
  }
  if (digitalRead(connect_wire1) == HIGH || digitalRead(connect_wire2) == HIGH){
    digitalWrite(connect_led_pin, LOW); 
    connectWireFinished = 0;
  }
}

void big_button_module(){
  if (get_green_button_press() && get_red_button_press() && membraneFinished && toggleFinished && rgbFinished && keypadFinished && connectWireFinished){
    Serial.println("big button finished");
    bigButtonFinished = 1;
    SoftPWMSetPercent(green_button_led_pin, 50);
  }
}

void finish_module(int endTime){
  if (membraneFinished && toggleFinished && rgbFinished && keypadFinished && connectWireFinished && bigButtonFinished && cutWireFinished && allFinished ==0){
    allFinished = 1;
    finishTime = get_time();
    //display.showNumberDecEx(0723, 0b11100000, false, 4, 0);
    display.clear();
    display.showNumberDec(723,false);
  }
  if (allFinished && get_green_button_press()){
    //display.showNumberDec(endTime,false, 4, 0);
    display.clear();
    display.showNumberDecEx(endTime, 0b11100000, false, 4, 0);
  }
  if (allFinished && get_red_button_press()){
    //display.showNumberDec(0723,false, 4, 0);
    display.clear();
    display.showNumberDec(723,false);
  }
}
