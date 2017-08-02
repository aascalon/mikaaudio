#include <SimpleTimer.h>

// Arrays for the LED pins
int LEDPins[9] = {22,24,26,28,30,32,34,36,38};
int modeLEDs[3] = {3,4,5};
int inputLEDs[2] = {8,9};

//encoder pin assignments
int pinA = 18;
int pinB = 19;
int modePin = 6;

//encoder variables
volatile int encoderLvl = 0;
unsigned int lastPos = 1;
bool A_stat = 0;
bool B_stat = 0;

//audio levels
//0 = volume
//1 = bass
//2 = treble
int vol = 50;
int bass = 0;
int treble = 0;
int* currLevel;
int mode = 0;

//input mode
//0 = aux
//1 = bluetooth
int input = 0;
int inputPin = 10;

//timer
SimpleTimer timer;

//battery indication
int batteryLvl = 76;
int batteryPin = 12;

void setup() {
  // put your setup code here, to run once:
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  pinMode(modePin, INPUT);
  pinMode(inputPin, INPUT);
  pinMode(batteryPin, INPUT);
  for(int i = 0; i <9; i++){
    pinMode(LEDPins[i],OUTPUT);
  }
 for(int i = 0; i <3; i++){
    pinMode(modeLEDs[i],OUTPUT);
  }
  for(int i = 0; i <2; i++){
    pinMode(inputLEDs[i],OUTPUT);
  }
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, HIGH);


  attachInterrupt(digitalPinToInterrupt(18), encoderA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(19), encoderB, CHANGE);
  Serial.begin(9600);
  updateMode();
  updateInput();
  timer.setInterval(5000,back2vol);
}

void loop() {
  timer.run();
  // put your main code here, to run repeatedly:
   if (lastPos != *currLevel) {
    Serial.print("current level:");
    Serial.print(*currLevel, DEC);
    Serial.println();
    lastPos = *currLevel;
  }
  

   if(digitalRead(modePin) == HIGH){
        delay(300);
        mode = ++mode%3;
        updateMode();
        Serial.print("current level:");
        Serial.print(*currLevel);
        Serial.println();
   }
   if(digitalRead(inputPin) == HIGH){
        delay(300);
        input = ++input%2;
        updateInput();
   }
   if(digitalRead(batteryPin) == HIGH){ 
        showBattery();
        Serial.print("battery button pressd");
        Serial.println();
   }
    

}


void encoderA(){
  A_stat = (digitalRead(pinA) == HIGH);
  if (A_stat != B_stat)
    (*currLevel)++;
  else
   (*currLevel)--;
   if (mode == 0)
    *currLevel = constrain(*currLevel, 0, 100);
   else
    *currLevel = constrain(*currLevel, -50, 50);

   updatelevelLEDs();
   timer.restartTimer(0);
}

void encoderB(){
 B_stat = (digitalRead(pinB) == HIGH);
  if (B_stat == A_stat)
    (*currLevel)++;
  else
    (*currLevel)--;
  if (mode == 0)
    *currLevel = constrain(*currLevel, 0, 100);
  else
    *currLevel = constrain(*currLevel, -50, 50);

  updatelevelLEDs();
  timer.restartTimer(0);


}

void updatelevelLEDs(){
  //volume levels
  if (mode == 0){
    for(int currLED = 0; currLED < 9 ; currLED++){
      if(currLED < (vol / 10.0))
        digitalWrite(LEDPins[currLED], HIGH);
      else
        digitalWrite(LEDPins[currLED], LOW);
      }
  }
 //bass/treble levels 
 else{
      digitalWrite(LEDPins[4], HIGH);
        if (*currLevel >= 0){
       
          for (int currLED = 0; currLED < 4; currLED++){
              digitalWrite(LEDPins[currLED],LOW);
          }
    
          for(int currLED = 5; currLED < 9; currLED++){
            if (currLED < (*currLevel/10.0)+5)
              digitalWrite(LEDPins[currLED],HIGH);
            else
              digitalWrite(LEDPins[currLED],LOW);
          } 
        }
    //level less than 0
       else{ 
         for (int currLED = 8; currLED > 4 ; currLED--){
          digitalWrite(LEDPins[currLED], LOW);
         }
         for (int currLED  = 3 ; currLED >=0 ; currLED--){
           if((currLED*10)-40 > *currLevel)
              digitalWrite(LEDPins[currLED], HIGH);
           else
           digitalWrite(LEDPins[currLED], LOW);
        }
     }
   }
}

void updateMode(){
  //handles the levels and mode LEDs
    timer.restartTimer(0);

    if (mode == 0){
       currLevel = &vol;
       Serial.print("volume mode active");
       Serial.println();

    }
    else if (mode == 1){
       currLevel = &bass;
       Serial.print("bass mode active");
       Serial.println();
    }
    else if (mode == 2){
      currLevel = &treble;
      Serial.print("treble mode active");
      Serial.println();
    }
    
    updatelevelLEDs();
    
    digitalWrite(modeLEDs[mode], HIGH);
    digitalWrite(modeLEDs[(mode+1)%3], LOW);
    digitalWrite(modeLEDs[(mode+2)%3], LOW);
}

void updateInput(){
  digitalWrite(inputLEDs[input], HIGH);
  digitalWrite(inputLEDs[(input+1) % 2], LOW);
}

void back2vol(){
  if (mode != 1 || mode !=2)
      mode = 0;
  updateMode();
  
}

void showBattery(){
  digitalWrite(modeLEDs[mode], LOW);
  for (int currLED = 0; currLED < 9; currLED++){
    digitalWrite(LEDPins[currLED], LOW);
  }
  delay(500);
  for (int currLED = 0; currLED < 9; currLED++){
    if (currLED < batteryLvl/10)
      digitalWrite(LEDPins[currLED], HIGH);
    delay(50);
  }
  delay(2000);
  updateMode();

}


