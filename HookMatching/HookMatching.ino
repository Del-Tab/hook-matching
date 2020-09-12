
#include "hm_music.hpp"

#include "HookDef.hpp"
#define speaker 22  // I'm using digital pin 22 on my mega2560
#define stateLed 23 // digital pin 23 will be on on playing state
#define pulseLed 2  // pwm pin 2 will give the pulse


struct scale GAMME_Do = {0, 0, NOTE_DO, "Do majeur"};

// fa est #, note de base = mi
struct scale miMineur = {B1000,0, NOTE_MI, "mi mineur"};

// fa est diese
struct scale solMajeur = {B1000, 0, NOTE_SOL, "sol majeur"};


struct scale miMajeur = {B11011, 0, NOTE_MI, "mi majeur"};

// E minor scale, 120 bpm (unit = halfbeat), 3/2 time signature
// gamme de mi mineur, 120 bpm (unité de battement = blanche), rythme = 3/2 (3 temps par mesure, unité dde temps = blanche) 
struct sheet Tourdion = {&miMineur, 120, 48, 3, 2};

struct hm_ctx dummyhmctx = {&Tourdion, &miMineur};


#define MAX_DEPTH 16
void dummyPlay(int8_t note, Playable *p) {
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


  Playable *blanche = new Note(48);             //half
  Playable *noire = new Note(24);               //quarter
  Playable *croche = new Note(12);              //eight's
  Playable *blanchePointee = new Note(72);      //dottedHalf
  Playable *ronde = new Note(96);               //full
  Playable *rondePointee = new Note(144);       //dottedFull
  Playable *fullCase = new Note(dummyhmctx.sheetInfo->top * dummyhmctx.sheetInfo->bottom * 24);
  Playable *hook1_1 = (new ListHook(6))->add(noire)
                                     ->add(noire, 1)
                                     ->add(noire, 2)
                                     ->add(noire, 3)
                                     ->add(noire, 2)
                                     ->add(noire, 1);
  Playable *hook1_2 = (new ListHook(7))->add(noire) 
                                     ->add(noire,  1)
                                     ->add(noire, -1)
                                     ->add(blanche)
                                     ->add(noire,   -1)
                                     ->add(noire,   -2)
                                     ->add(blanche, -3);
  Playable *hook1_3 = (new ListHook(6))->add(blanche)
                                     ->add(blanche,  2)
                                     ->add(blanche,  1)
                                     ->add(ronde)
                                     ->add(blanche, -1)
                                     ->add(rondePointee);
  Playable *firstPhrase = (new ListHook(6))->add(hook1_1)
                                           ->add(blanchePointee)
                                           ->add(hook1_1, 1)
                                           ->add(hook1_2, 2)
                                           ->add(hook1_1)
                                           ->add(hook1_3);
  Playable *hook2_1 = (new ListHook(4))->add(blanchePointee)
                                       ->add(noire, -1)
                                       ->add(noire)
                                       ->add(noire,  1);
  Playable *hook2_2 = (new ListHook(11))->add(ronde)
                                        ->add(blanche)
                                        ->add(noire,  2)
                                        ->add(noire, 1, NOTE_FORCE_SHARP) // add sharp
                                        ->add(noire)
                                        ->add(noire, -1)
                                        ->add(noire, -2)
                                        ->add(noire, -3)
                                        ->add(blanchePointee, -2)
                                        ->add(noire, -3)
                                        ->add(blanche, -4)
                          ;
  Playable *test = new RepeatHook(hook2_2);
  Playable *hook2_3 = (new ListHook(7))->add(blanchePointee)
                                      ->add(noire, -1)
                                      ->add(noire, -2)
                                      ->add(noire, -3)
                                      ->add(ronde, -4)
                                      ->add(blanche, -5)
                                      ->add(rondePointee, -4);
  Playable *secondPhrase = (new ListHook(4))->add(hook2_1)
                                            ->add(hook2_2)
                                            ->add(hook2_1)
                                            ->add(hook2_3);
  
  Playable *fullTourdion = (new ListHook(3))->add(new RepeatHook(firstPhrase, 2))
                                             ->add(new RepeatHook(secondPhrase, 2), 4)
                                             ->add(firstPhrase);
                                             
                                            
  
  Playable *beat = new RepeatHook(
                         (new ListHook(3))->add(blanche, 16)
                                          ->add(blanche, 4)
                                          ->add(blanche, 4)
                                 );
void setup() {
  
  pinMode(speaker, OUTPUT);
  pinMode(stateLed, OUTPUT);
  pinMode(pulseLed, OUTPUT);
  Serial.begin(9600);

  // play some jingle in the set scale
  ListHook *suite = new ListHook(6);
  suite->add(blanche);
  for (int i = 1; i <  5; ++i)
    suite->add(croche, i);
  suite->add(blanche,  5 );
  dummyPlay(NOTE_DO, suite);

  delay(2000);
  digitalWrite(stateLed, HIGH);
}
uint8_t coordinates[MAX_DEPTH] = {0};
uint8_t beatCoordinates[MAX_DEPTH] = {0};
unsigned long nextPlayedNode = 0;
unsigned long nextPlayedBeat = 0;
float brightness = 0;
Playable *playMe = (new ListHook(4))->add(fullCase, 0, NOTE_IS_SILENCE)->add(fullCase, 0, NOTE_IS_SILENCE)
                                    //->add(test,4)
                                    ->add(fullTourdion)
                                    ->add(fullCase, 0, NOTE_IS_SILENCE);
int state = 0;//playing
void loop() {
// put your main code here, to run repeatedly:
  //fade by step
  if (brightness > 0) {
    brightness -=.025;
    if (brightness < 0)
      brightness = 0;
    analogWrite(pulseLed, brightness);
  }
  if (state == 0) {
    unsigned long currentMillis = millis();
    
    if (nextPlayedNode <= currentMillis) {
      if (playMe->hasMore(coordinates, MAX_DEPTH, 0)) {
        note_info ni = playMe->getOne(coordinates, MAX_DEPTH, 0);
        // main hook is expressed in base note from the scale
        int8_t transpose;
        transpose = 0;
        Serial.print("flags = ");Serial.println(ni.flags);

       
        if ((ni.flags & NOTE_FORCE_SHARP) && (!isSharp(dummyhmctx.scaleInfo, ni.degreeOffset)))
          ++transpose;
        if ((ni.flags & NOTE_FORCE_FLAT) && (!isFlat(dummyhmctx.scaleInfo, ni.degreeOffset)))
          --transpose;
        if ((ni.flags & NOTE_FORCE_NATURAL)) {
          if (isFlat(dummyhmctx.scaleInfo, ni.degreeOffset))
            ++transpose;
          if (isSharp(dummyhmctx.scaleInfo, ni.degreeOffset))
            --transpose;
        }
        Serial.print("transpose = ");Serial.println(transpose);
        float freq = getFrequency( ni.degreeOffset, 5 + ni.octaveOffset, dummyhmctx.scaleInfo, transpose);
        uint32_t dur = getNoteLengthMillis(ni.duration, *dummyhmctx.sheetInfo);

        if ((ni.flags & NOTE_IS_SILENCE) == 0)
          tone(speaker, freq, .955*dur);
      
        
        // we take into accound this loop's calculus time
        nextPlayedNode = currentMillis + dur;
      }else {
        state = 1;
        digitalWrite(stateLed,LOW);
      }
    }
    if (nextPlayedBeat <= currentMillis) {
      note_info ni = beat->getOne(beatCoordinates, MAX_DEPTH, 0);
      uint32_t dur = getNoteLengthMillis(ni.duration, *dummyhmctx.sheetInfo);
      brightness = map((uint8_t) ni.degreeOffset, 0, 16, 0, 255);
      analogWrite(pulseLed, brightness);
      nextPlayedBeat = currentMillis + dur;
    }
  }  
}
