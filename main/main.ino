
#include "hook_matching.hpp"


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

// fa et do sont #, note de base = mi
struct scale miMineur = {B1001,0, NOTE_MI, "mi mineur"};

// gamme de mi mineur, 120 bpm (unité de battement = blanche), rythme = 3/2 (3 temps par mesure, unité dde temps = blanche) 
struct sheet Tourdion = {&miMineur, 120, 48, 3, 2};

// un hook est une suite de notes, pour l'instant on écrit le nombre de notes du hook,
// puis un array de couple "offset dans le hook en note sur la portée", "durée logique où la noire vaut 24"
// je pense qu'on peut faire mieux pour représenter les suites de notes, réflexion en cours

struct scale_hook blanchePointee = { 1, {{0, 72}} };
struct scale_hook hook1 = { 6, {
                                {0, 24},
                                {1, 24},
                                {2, 24},
                                {3, 24},
                                {2, 24},
                                {1, 24}
                               }
                          };
struct scale_hook hook2 = {7, {
                               {0, 24},
                               {1, 24},
                               {-1, 24},
                               {0, 48},
                               {-1, 24},
                               {-2, 24},
                               {-3, 48}  
                              }
                           };
struct scale_hook hook3= {6, {
                                {0, 48},
                                {2, 48},
                                {1, 48},
                                {0, 96},
                                {-1, 48},
                                {0, 144}
                               }
                          };
// une suite de hook + portée et note de référence du hook.
// Ça permet de réutiliser des motifs pour gagner de la  place.
contextual_scale_hook partition[] ={
  {5, NOTE_MI, &hook1},
  {5, NOTE_MI, &blanchePointee},
  {5, NOTE_FA, &hook1},
  {5, NOTE_SOL, &hook2},
  {5, NOTE_MI, &hook1},
  {5, NOTE_MI, &hook3}
};


void setup() {
  
  pinMode(speaker, OUTPUT);
  Serial.begin(9600);

  delay(2000);
  for (int i = 0; i < sizeof partition / sizeof partition[0]; ++i) {
    play(speaker, partition[i], Tourdion);
  }
  
  delay (2000);
  struct scale *scales[] = {// &GAMME_Do,
                            //&GAMME_Dom,
                          //&GAMME_DesertDom,
                         // &GAMME_JazzDo,
                         // &GAMME_JazzDo2,
                         // &Jazzy3,
                         // &GAMME_La,
                          //&GAMME_Lam
                          };

                          
  for (int i = 0; i < sizeof scales/sizeof (struct scale*); ++i) {
    Serial.print("\n------\nnow playing: ");
    Serial.println(scales[i]->display_name);
    //LA4_REF = 440.0; // already the default
    for (int j = -16; j < 16; ++j) {
      
      float freq = getFrequency(j, 5, scales[i]);
      Serial.print(freq);
      Serial.print(",");
      tone(speaker, round(freq), 250);
      delay(250);
    }
    /*
    LA4_REF = 460;
    for (int j = -16; j < 16; ++j) {
      
      float freq = getFrequency(j, 5, scales[i]);
      Serial.print(freq);
      Serial.print(",");
      tone(speaker, round(freq), 250);
      delay(250);
    }
    delay(1000);
    */
  }
}

void loop() {
// put your main code here, to run repeatedly:
  
  delay(10000);  
}
