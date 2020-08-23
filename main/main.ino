
#include "hook_matching.h"


#define speaker 2 // I'm using digital pin 2 on my mega2560

struct scale GAMME_Do = {0, 0, NOTE_DO, "Do majeur"};
struct scale GAMME_Dom = {0, B1100100, NOTE_DO, "Do mineur"};
struct scale GAMME_DesertDom = {0, B100100, NOTE_DO , "Do senteur desert"};
struct scale GAMME_JazzDo = {// do, ré#, mi#, fa#, sol, lab,sib 
 B1110, B1100000, NOTE_DO, "Jazzy Do"
};
struct scale GAMME_JazzDo2 ={ //dob, ré#, mi#, fa#, sol, la#,si#
  B1101110, B1, NOTE_DO, "Jazzy Do v2"
};
struct scale Jazzy3 = {B101110, 0, NOTE_DO, "jazzy 3"};
struct scale GAMME_La = {B11001, 0, NOTE_LA, "La majeur"};
struct scale GAMME_Lam = {0, 0, NOTE_LA, "La mineur"};


void setup() {
  Serial.begin(9600);

  delay (2000);
  struct scale *scales[] = { &GAMME_Do,
                            &GAMME_Dom,
                          //&GAMME_DesertDom,
                         // &GAMME_JazzDo,
                         // &GAMME_JazzDo2,
                         // &Jazzy3,
                         // &GAMME_La,
                        //  &GAMME_Lam
                          };

                          
  for (int i = 0; i < sizeof scales/sizeof (struct scale*); ++i) {
    Serial.print("\n------\nnow playing: ");
    Serial.println(scales[i]->display_name);
    //LA4_REF = 440.0; // already the default
    for (int j = -8; j < 8; ++j) {
      
      float freq = getFrequency(j, 5, *scales[i]);
      Serial.print(freq);
      Serial.print(",");
      tone(speaker, round(freq), 250);
      delay(250);
    }
    LA4_REF = 460;
    for (int j = -8; j < 8; ++j) {
      
      float freq = getFrequency(j, 5, *scales[i]);
      Serial.print(freq);
      Serial.print(",");
      tone(speaker, round(freq), 250);
      delay(250);
    }
  delay(1000);
  }
}

void loop() {
// put your main code here, to run repeatedly:
  
  delay(10000);  
}
