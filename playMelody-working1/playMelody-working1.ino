#include "pitches.h"  //add note library

int speaker = 9;
int buttonPin= 2;
bool activate = 0;

void setup(){

  //make the button's pin input
  pinMode(buttonPin, INPUT);
  pinMode(7, OUTPUT);
  attachInterrupt(0, sys_start, RISING);
}


void loop(){

  if (activate){

    //iterate over the notes of the melody
    for (int i=0; i < 5; i++){

      //to calculate the note duration, take one second. Divided by the note type
      int noteDuration = 1000 / 5;
      tone(speaker, NOTE_C5, 200);
      //to distinguish the notes, set a minimum time between them
      //the note's duration +30% seems to work well
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(250);
      tone(speaker, NOTE_FS4, 200);
      delay(250);
      noTone(9);
      delay(500);   
    }
    activate = 0;
  }
}

void sys_start(){
  activate = 1;

}
