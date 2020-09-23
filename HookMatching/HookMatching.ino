
#include "hm_music.hpp"
#include "HookDef.hpp"
#include <Tone.h>
#define voicePin1 22  // I'm using digital pin 22 on my mega2560
#define voicePin2 24
#define stateLed 23 // digital pin 23 will be on on playing state
#define pulseLed 2  // pwm pin 2 will give the pulse
Tone voice1,voice2;

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
    /*Serial.print("coordinates1 = {");
    for (int i = 0; i < MAX_DEPTH; ++i){
      Serial.print(coordinates1[i]);
      Serial.print(", ");
    }
    Serial.println("}");*/
    note_info ni = p->getOne(coordinates, MAX_DEPTH, 0);
    float freq = getFrequency(note - dummyhmctx.scaleInfo->note_base + ni.degreeOffset, 5 + ni.octaveOffset, dummyhmctx.scaleInfo);
    uint32_t dur = getNoteLengthMillis(ni.duration, *dummyhmctx.sheetInfo);
     
    voice1.play(freq, .955*dur);
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
  
  Playable *fullAlto = (new ListHook(3))->add(new RepeatHook(firstPhrase, 2))
                                             ->add(new RepeatHook(secondPhrase, 2), 4)
                                             ->add(firstPhrase);
                                             
  Playable *b_Hook1_1 = (new ListHook(2))->add(ronde)
                                         ->add(blanche);

  Playable *b_Hook1_2 = (new ListHook(5))->add(blanche)
                                         ->add(blanche, 2)
                                         ->add(blanche, 1)
                                         ->add(blanche, -2)
                                         ->add(ronde, -1);
  Playable *b_Hook1_3 = (new ListHook(5))->add(ronde)
                                         ->add(blanche, -1)
                                         ->add(blanche)
                                         ->add(ronde, -3)
                                         ->add(rondePointee);
  Playable *b_Hook2_1 = (new ListHook(5))->add(ronde)
                                         ->add(blanche, -3)
                                         ->add(blanche, -2)
                                         ->add(ronde, -5)
                                         ->add(rondePointee, -2);
  Playable *b_firstPhrase = (new ListHook(4))->add(new RepeatHook(b_Hook1_1, 2))
                                             ->add(b_Hook1_2, 2)
                                             ->add(b_Hook1_1)
                                             ->add(b_Hook1_3);
  Playable *b_secondPhrase = (new ListHook(6))->add(b_Hook1_1)
                                              ->add(b_Hook1_1, 2)
                                              ->add(b_Hook1_1, -1)
                                              ->add(b_Hook1_1)
                                              ->add(b_Hook1_1, 2)
                                              ->add(b_Hook2_1, 2);
  Playable *fullHommes = (new ListHook(3))->add(new RepeatHook(b_firstPhrase, 2))
                                             ->add(new RepeatHook(b_secondPhrase, 2))
                                             ->add(b_firstPhrase);                                           
                                              
                                          
  Playable *beat = new RepeatHook(
                         (new ListHook(3))->add(blanche, 16)
                                          ->add(blanche, 4)
                                          ->add(blanche, 4)
                                 );
void setup() {
  voice1.begin(voicePin1);
  voice2.begin(voicePin2);
  
  pinMode(voicePin1, OUTPUT);
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
uint8_t coordinates1[MAX_DEPTH] = {0};
uint8_t coordinates2[MAX_DEPTH] = {0};
uint8_t beatCoordinates[MAX_DEPTH] = {0};
unsigned long nextPlayedNote1 = 0;
unsigned long nextPlayedNote2 = 0;
unsigned long nextPlayedBeat = 0;
float brightness = 0;
Playable *playVoice1 = (new ListHook(4))->add(fullCase, 0, NOTE_IS_SILENCE)->add(fullCase, 0, NOTE_IS_SILENCE)
                                    //->add(test,4)
                                    ->add(fullAlto)
                                    ->add(fullCase, 0, NOTE_IS_SILENCE);
Playable *playVoice2 = (new ListHook(4))->add(fullCase, 0, NOTE_IS_SILENCE)->add(fullCase, 0, NOTE_IS_SILENCE)
                                    ->add(fullHommes);

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
    
    if (nextPlayedNote1 <= currentMillis) {
      if (playVoice1->hasMore(coordinates1, MAX_DEPTH, 0)) {
        note_info ni = playVoice1->getOne(coordinates1, MAX_DEPTH, 0);
        // main hook is expressed in base note from the scale
        int8_t transpose;
        transpose = 0;

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
        float freq = getFrequency( ni.degreeOffset, 6 + ni.octaveOffset, dummyhmctx.scaleInfo, transpose);
        uint32_t dur = getNoteLengthMillis(ni.duration, *dummyhmctx.sheetInfo);

        if ((ni.flags & NOTE_IS_SILENCE) == 0)
          voice1.play(freq, .955*dur);
      
        
        // we take into accound this loop's calculus time
        nextPlayedNote1 = currentMillis + dur;
      }else {
        state = 1;
        digitalWrite(stateLed,LOW);
      }
    }
    if (nextPlayedNote2 <= currentMillis) {
      if (playVoice2->hasMore(coordinates2, MAX_DEPTH, 0)) {
        note_info ni = playVoice2->getOne(coordinates2, MAX_DEPTH, 0);
        // main hook is expressed in base note from the scale
        int8_t transpose;
        transpose = 0;

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
        float freq = getFrequency( ni.degreeOffset, 5 + ni.octaveOffset, dummyhmctx.scaleInfo, transpose);
        uint32_t dur = getNoteLengthMillis(ni.duration, *dummyhmctx.sheetInfo);

        if ((ni.flags & NOTE_IS_SILENCE) == 0)
          voice2.play(freq, .955*dur);
      
        
        // we take into accound this loop's calculus time
        nextPlayedNote2 = currentMillis + dur;
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
