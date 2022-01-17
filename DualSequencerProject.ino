//ANDRES TINOCO 216323560 ARDUINO PIANO DUAL SEQUENCER FOR EECS2032
//APRIL 29, 2021    C++

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IF THE PROGRAM DOES NOT RUN MAKE SURE TO ADD THE ZIP FOLDER NAMED LiquidCrystal_I2C-master INTO THIS SKETCH //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <LiquidCrystal_I2C.h>
//LiquidCrystal library used to display LCD using arduino
//define lcd display for lcd size specifications
LiquidCrystal_I2C lcd(0x27,20,4);

//define pins for LED
#define LED1 53
#define LED2 52
#define LED3 51
#define LED4 50
#define LED5 49
#define LED6 48
#define LED7 47
#define LED8 46

//define pins for switches
#define Switch1 2
#define Switch2 3
#define Switch3 4
#define Switch4 5
#define Switch5 6
#define Switch6 7
#define Switch7 8
#define Switch8 9
#define StopSwitch 10
#define patternSwitch 13

//define pin to output signal
#define DSignal 11

//define pins for reading potentiometer values
#define ALength 0
#define AFrequency 1
#define ATempo 2

//notes array stores two patterns [0] or [1] with frequency values that play the song mary had a little lamb
int notes[2][8] = {{329,293,261,293,329,329,329,329},{294,294,294,294,329,392,392,392}};

//toneLength, Freqo, tempo will hold potentiometer values
int toneLength;
int Freqo;
int tempo;

//noteFreq holds the converted Freqo value to the nearest musical note frequency
int noteFreq;

//patt is used to choose between patter 1 or 2 from the 2D array [0][8] or [1][8]
int patt=false;
//switchPressed keeps track of what switches have been pressed to display to the user using the LED's
int SwitchPressed = -1;
//hold value will hold the previous switch pressed to turn off the LED accordingly
int hold=0;

//setup LCD, LED's, Switches and where signal will be outputted
void setup()
{
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();

 //define LED's and Switches
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);
  pinMode(LED4,OUTPUT);
  pinMode(LED5,OUTPUT);
  pinMode(LED6,OUTPUT);
  pinMode(LED7,OUTPUT);
  pinMode(LED8,OUTPUT);
  pinMode (Switch1, INPUT);
  pinMode (Switch2, INPUT);
  pinMode (Switch3, INPUT);
  pinMode (Switch4, INPUT);
  pinMode (Switch5, INPUT);
  pinMode (Switch6, INPUT);
  pinMode (Switch7, INPUT);
  pinMode (Switch8, INPUT);               
  pinMode (StopSwitch, INPUT);
  //Signal is outputted to pin 11
  pinMode (DSignal, OUTPUT); 

}


void loop()
{
  //check if the user has used the switch to pause/play
  if (digitalRead (StopSwitch)){
    for (int i=0; i<8; i++){  
      //check switches, update potentiometers and update display
      switchCheck();
      updatePot();
      updateDisplay();
    
      //add 6 for pins 53->46 turns on 8 LED's in order 
      digitalWrite(i+46,HIGH);
      //freqOut function takes frequency values and plays them by creating a square wave
      //playing the note for the desired toneLength in milliseconds
      freqOut(notes[patt][i], toneLength);
      digitalWrite(i+46,LOW);
      //delay in milliseconds between each note
      delay (tempo);      
    }  
    //checks if switch was pressed and switches pattern after 8 notes are played
    if (digitalRead (patternSwitch) == HIGH){
      //since patt is either 0 or 1, when the switch is pressed it changes which row in the 2D array is accessed [0][8] or [1][8]
      patt=!patt;
      //resets the switchPressed so the user can visually see that this is a new pattern
      SwitchPressed=-1;
    }
  }
}

// Checks each switch to see if it has been pressed
void switchCheck(){
  //noteFreq  takes the current potentiometer frequency values and converts it to the nearest musical note corresponding to it
  noteFreq=freqToNote(analogRead(AFrequency));

  //using a for loop that goes through pins 0->7 checks each switch, 
  //if it has been pressed it assigns the frequency value into the corresponding place in the 2D array
  for(int k=0;k<8;k++){
    if (digitalRead (k+2) == HIGH){
      notes[patt][k] = noteFreq;
      SwitchPressed = k+1;
    }
  }
}

//updates potentiometer values for tempo, toneLength and frequency
void updatePot(){
    tempo = analogRead (ATempo);
    toneLength = analogRead (ALength);
    //if the frequency is lower than 70 it is set to zero for an EMPTY space 
    if(analogRead(AFrequency)<70) Freqo=0;
    else Freqo=analogRead(AFrequency);
}

//updates lcd display for current tempo, toneLength, frequency 
//and the current musical note the frequency is at from A->G includes A#->G#
void updateDisplay(){
  //pushedLED holds the switch that was pressed plus 45 to turn on the corresponding LED
  int pushedLED=SwitchPressed+45;
  
  //Set LCD cursor to top left of screen to print tempo
  lcd.setCursor(0,0); lcd.print("t:"); lcd.print(tempo); lcd.print(" ");
  
  //Set LCD cursor to top middle of screen to print frequency
  lcd.setCursor(7,0); lcd.print("f:"); lcd.print(Freqo); lcd.print(" ");
  
  //Set LCD cursor to bottom left of screen to print toneLength
  lcd.setCursor(0,1); lcd.print("d:"); lcd.print(toneLength); lcd.print(" ");

  //Set LCD cursor to bottom middle of screen to print the current approximate musical note
  //and display which pattern is playing (1 or 2)
  lcd.setCursor(7,1);
  
  //if the frequency is in between a certain amount it is set to the corresponding note  
  //with the neaerest octave, if it is less than 100 the note will be EMPTY
  if (Freqo<100)  lcd.print("EMPTY p:");
  else if ((Freqo>100&&Freqo<=106)||(Freqo>204&&Freqo<=210)||(Freqo>412&&Freqo<=430)||(Freqo>827&&Freqo<=850))  lcd.print("G#    p:");
  else if ((Freqo>106&&Freqo<=113)||(Freqo>210&&Freqo<=225)||(Freqo>430&&Freqo<=450)||(Freqo>850&&Freqo<=910))  lcd.print("A     p:");
  else if ((Freqo>113&&Freqo<=118)||(Freqo>230&&Freqo<=240)||(Freqo>450&&Freqo<=480)||(Freqo>910&&Freqo<=1300)) lcd.print("A#    p:");
  else if ((Freqo>118&&Freqo<=126)||(Freqo>240&&Freqo<=255)||(Freqo>480&&Freqo<=508)||(Freqo>960&&Freqo<=960))  lcd.print("B     p:");
  else if ((Freqo>126&&Freqo<=134)||(Freqo>255&&Freqo<=265)||(Freqo>508&&Freqo<=533)) lcd.print("C     p:");
  else if ((Freqo>134&&Freqo<=142)||(Freqo>265&&Freqo<=269)||(Freqo>533&&Freqo<=570)) lcd.print("C#    p:");
  else if ((Freqo>142&&Freqo<=150)||(Freqo>269&&Freqo<=305)||(Freqo>570&&Freqo<=602)) lcd.print("D     p:");
  else if ((Freqo>150&&Freqo<=160)||(Freqo>305&&Freqo<=320)||(Freqo>602&&Freqo<=640)) lcd.print("D#    p:");
  else if ((Freqo>160&&Freqo<=169)||(Freqo>320&&Freqo<=339)||(Freqo>640&&Freqo<=678)) lcd.print("E     p:");
  else if ((Freqo>169&&Freqo<=179)||(Freqo>339&&Freqo<=359)||(Freqo>678&&Freqo<=720)) lcd.print("F     p:");
  else if ((Freqo>179&&Freqo<=190)||(Freqo>359&&Freqo<=380)||(Freqo>720&&Freqo<=760)) lcd.print("F#    p:");
  else if ((Freqo>190&&Freqo<=204)||(Freqo>380&&Freqo<=412)||(Freqo>760&&Freqo<=827)) lcd.print("G     p:");
  
  //prints which pattern is currently playing
  lcd.print(patt+1);
  
  //if a switch has been pressed the hold value will turn off the previous LED and store the current LED that has been turned on
  //The new LED will be turned on according to which switch was pressed
  if (SwitchPressed != -1){
    digitalWrite(hold,LOW);
    hold=pushedLED;
    digitalWrite(pushedLED,HIGH);
  }
}

//freqToNote function converts the current frequency to a corresponding musical note (G#,A,A#,B,C,C#,D,D#,E,F,F#,G)
//returns the closest corresponding musical note in Hz
int freqToNote(int noteF){
  //if frequency is less than 100 it is set to zero for EMPTY space
  if (Freqo<100){
    noteF=0;
  }
  
  //G#     if the frequency is between these ranges it is considered G#
  //       The if statements inside choose whether it is a high or low octave the G# will be 
  //       This applies to all the other if statements in this function
  else if ((Freqo>100&&Freqo<=106)||(Freqo>204&&Freqo<=210)||(Freqo>412&&Freqo<=430)||(Freqo>827&&Freqo<=850)){
    //frequency assigned is approximately what the note should be using its Frequency with a frequency to note chart
    if(Freqo>100&&Freqo<=106) noteF=104;
    else if(Freqo>204&&Freqo<=210) noteF=207;
    else if(Freqo>412&&Freqo<=430) noteF=415;
    else if(Freqo>827&&Freqo<=850) noteF=830;
  }
  //A
  else if ((Freqo>106&&Freqo<=113)||(Freqo>210&&Freqo<=225)||(Freqo>430&&Freqo<=450)||(Freqo>850&&Freqo<=910)){
    if(Freqo>100&&Freqo<=106) noteF=110;
    else if(Freqo>210&&Freqo<=225) noteF=220;
    else if(Freqo>430&&Freqo<=450) noteF=440;
    else if(Freqo>850&&Freqo<=910) noteF=880;
  }
  //A#
  else if ((Freqo>113&&Freqo<=118)||(Freqo>230&&Freqo<=240)||(Freqo>450&&Freqo<=480)||(Freqo>910&&Freqo<=1300)){
    if(Freqo>113&&Freqo<=118) noteF=116;
    else if(Freqo>230&&Freqo<=240) noteF=233;
    else if(Freqo>450&&Freqo<=480) noteF=466;
    else if(Freqo>910&&Freqo<=960) noteF=932;
  }
  //B
  else if ((Freqo>118&&Freqo<=126)||(Freqo>240&&Freqo<=255)||(Freqo>480&&Freqo<=508)||(Freqo>960&&Freqo<=960)){
    if(Freqo>118&&Freqo<=126) noteF=123;
    else if(Freqo>240&&Freqo<=255) noteF=246;
    else if(Freqo>480&&Freqo<=508) noteF=493;
    else if(Freqo>960&&Freqo<=960) noteF=987;
  }
  //C
  else if ((Freqo>126&&Freqo<=134)||(Freqo>255&&Freqo<=265)||(Freqo>508&&Freqo<=533)){
    if(Freqo>126&&Freqo<=134) noteF=131;
    else if(Freqo>255&&Freqo<=265) noteF=261;
    else if(Freqo>480&&Freqo<=508) noteF=523;
  }
  //C#
  else if ((Freqo>134&&Freqo<=142)||(Freqo>265&&Freqo<=269)||(Freqo>533&&Freqo<=570)){
    if(Freqo>134&&Freqo<=142) noteF=138;
    else if(Freqo>265&&Freqo<=269) noteF=277;
    else if(Freqo>533&&Freqo<=570) noteF=554;
  }
  //D
  else if ((Freqo>142&&Freqo<=150)||(Freqo>269&&Freqo<=305)||(Freqo>570&&Freqo<=602)){
    if(Freqo>142&&Freqo<=150) noteF=147;
    else if(Freqo>269&&Freqo<=305) noteF=293;
    else if(Freqo>570&&Freqo<=602) noteF=587;
  }
  //D#
  else if ((Freqo>150&&Freqo<=160)||(Freqo>305&&Freqo<=320)||(Freqo>602&&Freqo<=640)){
    if(Freqo>150&&Freqo<=160) noteF=155;
    else if(Freqo>305&&Freqo<=320) noteF=311;
    else if(Freqo>602&&Freqo<=640) noteF=622;
  }
  //E
  else if ((Freqo>160&&Freqo<=169)||(Freqo>320&&Freqo<=339)||(Freqo>640&&Freqo<=678)){
    if(Freqo>160&&Freqo<=169) noteF=165;
    else if(Freqo>320&&Freqo<=339) noteF=329;
    else if(Freqo>640&&Freqo<=678) noteF=659;
  }
  //F
  else if ((Freqo>169&&Freqo<=179)||(Freqo>339&&Freqo<=359)||(Freqo>678&&Freqo<=720)){
    if(Freqo>169&&Freqo<=179) noteF=174;
    else if(Freqo>339&&Freqo<=359) noteF=349;
    else if(Freqo>678&&Freqo<=720) noteF=698;
  }
  //F#
  else if ((Freqo>179&&Freqo<=190)||(Freqo>359&&Freqo<=380)||(Freqo>720&&Freqo<=760)){
    if(Freqo>179&&Freqo<=190) noteF=185;
    else if(Freqo>359&&Freqo<=380) noteF=370;
    else if(Freqo>720&&Freqo<=760) noteF=740;
  }
  //G
  else if ((Freqo>190&&Freqo<=204)||(Freqo>380&&Freqo<=412)||(Freqo>760&&Freqo<=827)){
    if(Freqo>190&&Freqo<=204) noteF=196;
    else if(Freqo>380&&Freqo<=412) noteF=392;
    else if(Freqo>760&&Freqo<=827) noteF=784;
  }
  //returns the closest musical note in Hz 
  return noteF;
}

//freqOut function takes the current frequency and toneLength duration and converts it into a square wave
//using halfperiod, number of periods per millisecond
//halfperiod delays the signal for a certain amount of micro seconds before it is turned off
//this cycle is repeated for a certain number of periods to the desired amount of time the note should be played
void freqOut(long freq, long t){
  //halfperiod calculation (microseconds) using frequency(Hz)
  int halfPeriod = (500000 / (freq)); 
  //number of periods to play the note for in milliseconds     
  long numPeriods = (freq * t) / 1000;    

  //for loop to play the squarewave for the number of periods calculated
  for (long i=0; i <= numPeriods; i++){
    //outputs signal to pin 11
    digitalWrite(DSignal, HIGH); 
    //delay for halfperiod
    delayMicroseconds(halfPeriod);
    //turn off signal
    digitalWrite(DSignal, LOW); 
    delayMicroseconds(halfPeriod);     
  }
}
