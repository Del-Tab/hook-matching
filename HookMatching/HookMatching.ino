
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
struct hm_ctx dummyhmctx = {&Tourdion, &GAMME_Do};


#define MAX_DEPTH 16
void dummyPlay(int8_t note, Playable *p) {

  boolean noteDummy = true;
  uint8_t coordinates[MAX_DEPTH] = {0};
  while (p->hasMore(coordinates, MAX_DEPTH, 0)) {
    Serial.print("coordinates = {");
    for (int i = 0; i < MAX_DEPTH; ++i){
      Serial.print(coordinates[i]);
      Serial.print(", ");
    }
    Serial.println("}");
    note_info ni = p->getOne(coordinates, MAX_DEPTH, 0);
    float freq = getFrequency(dummyhmctx.scaleInfo->note_base + note + ni.degreeOffset, 5 + ni.octaveOffset, dummyhmctx.scaleInfo);
    uint32_t dur = getNoteLengthMillis(ni.duration, *dummyhmctx.sheetInfo);
     
    tone(speaker, freq, dur -15);
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

  struct default_play_context myCtx = {speaker};
  for (int i = 0; i < sizeof partition / sizeof partition[0]; ++i) {
    walk(&default_play_cb, &myCtx, partition[i], Tourdion);
  }
}

void loop() {
// put your main code here, to run repeatedly:
  
  delay(10000);  
}
