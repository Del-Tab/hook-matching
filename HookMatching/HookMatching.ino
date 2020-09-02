
#include "hm_music.hpp"

#include "HookDef.hpp"
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

struct scale miMajeur = {B11011, 0, NOTE_MI, "mi majeur"};

// gamme de mi mineur, 120 bpm (unité de battement = blanche), rythme = 3/2 (3 temps par mesure, unité dde temps = blanche) 
struct sheet Tourdion = {&miMineur, 120, 48, 3, 2};

struct hm_ctx dummyhmctx = {&Tourdion, &miMineur};


#define MAX_DEPTH 16
void dummyPlay(int8_t note, Playable *p) {

  boolean noteDummy = true;
  uint8_t coordinates[MAX_DEPTH] = {0};
  while (p->hasMore(coordinates, MAX_DEPTH, 0)) {
    /*Serial.print("coordinates = {");
    for (int i = 0; i < MAX_DEPTH; ++i){
      Serial.print(coordinates[i]);
      Serial.print(", ");
    }
    Serial.println("}");*/
    note_info ni = p->getOne(coordinates, MAX_DEPTH, 0);
    float freq = getFrequency(note - dummyhmctx.scaleInfo->note_base + ni.degreeOffset, 5 + ni.octaveOffset, dummyhmctx.scaleInfo);
    uint32_t dur = getNoteLengthMillis(ni.duration, *dummyhmctx.sheetInfo);
     
    tone(speaker, freq, .955*dur);
    delay(dur);
  }
}
void setup() {
  
  pinMode(speaker, OUTPUT);
  Serial.begin(9600);

  Playable *blanche = new Note(48);
  Playable *noire = new Note(24);
  Playable *croche = new Note(12);
  ListHook *suite = new ListHook(6);
  suite->add(blanche);
  for (int i = 1; i <  5; ++i)
    suite->add(croche, i);
  suite->add(blanche,  5 );
  dummyPlay(NOTE_DO, suite);

  delay(2000);
  Playable *blanchePointee = new Note(72);
  Playable *ronde = new Note(96);
  Playable *rondePointee = new Note(144);
  Playable *hook1 = (new ListHook(6))->add(noire)
                                     ->add(noire, 1)
                                     ->add(noire, 2)
                                     ->add(noire, 3)
                                     ->add(noire, 2)
                                     ->add(noire, 1);
  Playable *hook2 = (new ListHook(7))->add(noire) 
                                     ->add(noire,  1)
                                     ->add(noire, -1)
                                     ->add(blanche)
                                     ->add(noire,   -1)
                                     ->add(noire,   -2)
                                     ->add(blanche, -3);
  Playable *hook3 = (new ListHook(6))->add(blanche)
                                     ->add(blanche,  2)
                                     ->add(blanche,  1)
                                     ->add(ronde)
                                     ->add(blanche, -1)
                                     ->add(rondePointee);
  Playable *firstPhrase = (new ListHook(6))->add(hook1)
                                           ->add(blanchePointee)
                                           ->add(hook1, 1)
                                           ->add(hook2, 2)
                                           ->add(hook1)
                                           ->add(hook3);

  dummyPlay(NOTE_MI, firstPhrase);
}

void loop() {
// put your main code here, to run repeatedly:
  
  delay(10000);  
}
