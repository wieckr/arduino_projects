//4 Button Timer
/********************************
There are 4 buttons with separate timers.  1 push to start the timer for the event, 
double tap to stop the alarm (before or after it goes off). 
An LED goes off as well as a buzzer melody for the alarm.

Requires RTC module, 4 buttons, 4 LEDs, 1 buzzer, various resistors, breadboard
*********************************/


#include "Wire.h"
#define DS3231_I2C_ADDRESS 0x68
// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}

const int button1 = 3;   //Washer
//const int buttonPin = 3;
const int button2 = 8;   //Dryer
const int button3 = 7;   //Dishwasher
//const int button4 = 6;   //1 hour timer

const int timer1 = 2; //2 mins
const int timer2 = 2; 
const int timer3 = 2;
//const int timer4 = 60;

const int buzzPin = 12;  //buzzer is plugged in here

const int LEDRED = 11;
const int LEDBLUE = 9;
const int LEDGREEN = 10;



/*************************************************
 * Public Constants 
 * Notes for Buzz Song
 *************************************************/

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

#define melodyPin 12
//Mario main theme melody
int melody[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0,

  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,

  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0,

  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,

  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0
};
//Mario main them tempo
int tempo[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};
//Underworld melody
int underworld_melody[] = {
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0, NOTE_DS4, NOTE_CS4, NOTE_D4,
  NOTE_CS4, NOTE_DS4,
  NOTE_DS4, NOTE_GS3,
  NOTE_G3, NOTE_CS4,
  NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
  NOTE_GS4, NOTE_DS4, NOTE_B3,
  NOTE_AS3, NOTE_A3, NOTE_GS3,
  0, 0, 0
};
//Underwolrd tempo
int underworld_tempo[] = {
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  6, 18, 18, 18,
  6, 6,
  6, 6,
  6, 6,
  18, 18, 18, 18, 18, 18,
  10, 10, 10,
  10, 10, 10,
  3, 3, 3
};

 int timer_start1 = 0;
 int timer_start2 = 0;
 int timer_start3 = 0;
 //int timer_start4 = 0;
 //int current_time = 0;

 char button1_pushed = LOW;
 char button2_pushed = LOW;
 //char button3_pushed = LOW;
 //char button4_pushed = LOW;
 int button_pushed = 0;
void setup()
{
 Wire.begin();
 // set the initial time here:
 // DS3231 seconds, minutes, hours, day, date, month, year
 // setDS3231time(00,53,19,2,14,12,15);
 Serial.begin(9600);
 pinMode(button1, INPUT);
 pinMode(button2, INPUT);
 pinMode(button3, INPUT);
 //pinMode(button4, INPUT);
 pinMode(buzzPin, OUTPUT);

 pinMode(LEDRED, OUTPUT);
 pinMode(LEDBLUE, OUTPUT);
 pinMode(LEDGREEN, OUTPUT);

 



}

void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}
void readDS3231time(byte *second,
byte *minute,
byte *hour,
byte *dayOfWeek,
byte *dayOfMonth,
byte *month,
byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}
int displayTime()
{
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);
  // send it to the serial monitor
  Serial.print(hour, DEC);
  // convert the byte variable to a decimal number when displayed
  Serial.print(":");
  if (minute<10)
  {
    Serial.print("0");
  }
  Serial.print(minute, DEC);
  Serial.print("\n");
  Serial.print(hour*60 + minute);

  return (hour*60 + minute);
 
}

char check_button(){
  char but1 = LOW;
  char but2 = LOW;
  char but3 = LOW;
  char but4 = LOW;
 //for (int i = 1; i < 2; i++;){ 
//}
  //Serial.print(button1);
  if (digitalRead(button1) == HIGH){
    delay(200);
    if (digitalRead(button1) == HIGH){
       return 1;
    }
    else {
      delay(200);
      return 0;
    }
  }
  else if (digitalRead(button2) == HIGH){
    delay(200);
    if (digitalRead(button2) == HIGH){
       return 2;
    }
    else {
      delay(200);
      return 0;
    }
  }
  else if (digitalRead(button3) == HIGH){
    delay(200);
    if (digitalRead(button3) == HIGH){
       return 3;
    }
    else {
      delay(200);
      return 0;
    }
  }  
  else {
    return 0;
  }

  
}

void color (unsigned char red, unsigned char green, unsigned char blue)     // the color generating function  
{    
          analogWrite(LEDRED, red);   
          analogWrite(LEDBLUE, blue); 
          analogWrite(LEDGREEN, green); 
}

void check_timers(int current_time, int end_timer1, int end_timer2, int end_timer3){
/**************************************
 * Checks if any timers are done
 * Returns number of timer if complete
 * INPUT: start timers for all 4 timers
 * RETURNS: Number of done timer, otherwise 0
 *************************************/
 //Serial.print("Current time: ");
 //Serial.print(current_time);
 //Serial.print("\ntimer1: ");
 Serial.print("\nend timer: ");
 Serial.println(end_timer1);
 Serial.print("end of end timer\n");
 //Serial.print("\ntimer start: ");
 
 
 //Serial.print(timer_start1);
if ((current_time >= end_timer1) && end_timer1 != timer1){
  //make led light up
  Serial.print("Timer 1 is up!\n");
  color(255,0,0); //RED
  end_timer1 = 0;
  make_buzz(1);
  color(0,0,0);
}
else if ((current_time >= end_timer2) && end_timer2 != timer2){
  //make led light up
  Serial.print("Timer 2 is up!\n");
  color(0,0,255); //Blue
  end_timer2 = 0;
  make_buzz(2);
  color(0,0,0);
}
else if ((current_time >= end_timer3) && end_timer3 != timer3){
  //make led light up
  Serial.print("Timer 3 is up!\n");
  color(255,255,255); //White
  end_timer3 = 0;
  make_buzz(3);
  color(0,0,0);
}
 
}
void start_timer(int time_length, int timer_num){
  
}



void make_buzz(int button){
  //spit out buzz noise until button is pushed, then return if pushed.
  int finished = 0;
  Serial.print("make_buzz");
  do {
    finished = sing(button);
    //pushed = digitalRead(button1);
    //int current_time = displayTime();
    //check_timers(current_time, 1);
  } while (finished == 0);
  //analogWrite(ledPin, 0);
  return;
}


int song = 0;

int sing(int s) {
  // iterate over the notes of the melody:
  song = s;
  if (song == 2) {
    Serial.println(" 'Underworld Theme'");
    int size = sizeof(underworld_melody) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++) {
      if (digitalRead(button1) == HIGH || digitalRead(button2) == HIGH || digitalRead(button3) == HIGH){
        Serial.println("stopped alarm");
        return 1; 
      }
    
      // to calculate the note duration, take one second
      // divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / underworld_tempo[thisNote];

      buzz(melodyPin, underworld_melody[thisNote], noteDuration);

      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);

      // stop the tone playing:
      buzz(melodyPin, 0, noteDuration);

    }

  } else {

    Serial.println(" 'Mario Theme'");
    int size = sizeof(melody) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++) {
       if (digitalRead(button1) == HIGH || digitalRead(button2) == HIGH || digitalRead(button3) == HIGH){
        Serial.println("stopped alarm");
        return 1; 
      }

      // to calculate the note duration, take one second
      // divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / tempo[thisNote];

      buzz(melodyPin, melody[thisNote], noteDuration);

      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);

      // stop the tone playing:
      buzz(melodyPin, 0, noteDuration);

    }
  }
}

void buzz(int targetPin, long frequency, long length) {
  //digitalWrite(13, HIGH);
  long delayValue = 1000000 / frequency / 2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  long numCycles = frequency * length / 1000; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to
  //// get the total number of cycles to produce
  for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
    digitalWrite(targetPin, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin, LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait again or the calculated delay value
  }
  //digitalWrite(13, LOW);

}

void loop()
{
  int current_time = displayTime();
  do{
    current_time = displayTime();
    button_pushed = check_button();
    //button2_pushed = digitalRead(button2);
    //button3_pushed = digitalRead(button3);
    //button4_pushed = digitalRead(button4);
    check_timers(current_time, timer_start1+timer1, timer_start2+timer2, timer_start3+timer3);
    Serial.print("current time: ");
    Serial.println(current_time);
    Serial.println(timer_start1);
    Serial.println(button_pushed);
    //Serial.print(button1_pushed);
    //Serial.print(current_time);
    //current_time = displayTime();
    //Serial.print("\n");
    
  //} while(button1_pushed == LOW && button2_pushed == LOW && button3_pushed == LOW && button4_pushed == LOW)//no button pushes
  } while(button_pushed == 0);//no button pushes

  

  if (button_pushed == 1){
    Serial.print("button1 pushed");
    if (timer_start1 == 0 && current_time != 0){//not started yet
      Serial.print("Timer 1 Started\n");
      color(255,0,0); //RED
      timer_start1 = current_time; 
    }
    else if (timer_start1 == 0 && current_time == 0) {//midnight and started
      Serial.print("Timer 1 Started Midnight \n");
      color(255,0,0);
      timer_start1 = current_time;
    }
    else{
      Serial.print("Timer 1 Stopped\n");
      color(0,0,0);
      timer_start1 =0;   //stop alarm
    }
  }
  if (button_pushed == 2){
    Serial.print("button2 pushed");
    if (timer_start2 == 0 && current_time != 0){//not started yet
      Serial.print("Timer 2 Started\n");
      color(0,0,255); //Blue
      timer_start2 = current_time; 
    }
    else if (timer_start2 == 0 && current_time == 0) {//midnight and started
      Serial.print("Timer 2 Started Midnight \n");
      color(0,0,255);
      timer_start2 = current_time;
    }
    else{
      Serial.print("Timer 2 Stopped\n");
      color(0,0,0);
      timer_start2 =0;   //stop alarm
    }
  }
  if (button_pushed == 3){
    Serial.print("button3 pushed");
    if (timer_start3 == 0 && current_time != 0){//not started yet
      Serial.print("Timer 3 Started\n");
      color(255,255,255); //White
      timer_start3 = current_time; 
    }
    else if (timer_start3 == 0 && current_time == 0) {//midnight and started
      Serial.print("Timer 3 Started Midnight \n");
      color(255,255,255);
      timer_start3 = current_time;
    }
    else{
      Serial.print("Timer 3 Stopped\n");
      color(0,0,0);
      timer_start3 =0;   //stop alarm
    }
  }
  /*
  else if (button2_pushed == HIGH){
     current_time = get_time();
    if (timer_start2 == 0){/  /not started yet
      timer_start2 = current_time; 
    }
    else{
      cancel_timer(2);   //stop alarm
    }
  }
  else if (button3_pushed == HIGH){
    current_time = get_time();
    if (timer_start3 == 0){/  /not started yet
      timer_start3 = current_time; 
    }
    else{
      cancel_timer(3);   //stop alarm
    }
  }
  else if (button4_pushed == HIGH){
    current_time = get_time();
    if (timer_start4 == 0){/  /not started yet
      timer_start4 = current_time; 
    }
    else{
      cancel_timer(4);   //stop alarm
    }
  }
  */

 
}
