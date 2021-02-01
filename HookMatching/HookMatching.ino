
#include "hm_music.hpp"
#include "HookDef.hpp"
#include <avr/sleep.h>
#include "hm_scale.hpp"
/*
   This program is available, and maintained (maybe ;-) ) here: https://github.com/DelTa-B/hook-matching

   WARNING for this program to work (in current version) you need to download the Tone library by Brett Hagman
   You may find this library in the Arduino IDE market place (under Tools-> manage libraries)
   or download it on Brett Hagman's github here: https://github.com/bhagman/Tone
   on Arduino Mega 2560 R3, you must modify it to handle all the available timers
   - Open, from your Arduino install folder,
     the file libraries/Tone/Tone.cpp.
   - Replace every occurence of
     #if defined(__AVR_ATmega1280__)
     by
     #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
     then you can compile your sketch and send to your arduino :)
     This trick was not tested with more than 3 tone instances
       and it already breaks PWM.
*/
// pins for playing sound (one per voice) if you are using the TonePlayer implementation (which will be removed one day because number of voices is too limited)
static int voicePin1 = 22;
static int voicePin2 = 24;
static int voicePin3 = 26;
// pin for led telling which state we are in
#define stateLed 23 // digital pin 23 will be on on playing state

// pin for interrupts dealing with rotary encoder
static int CHA = 2; // must be an interrupt pin
static int CHB = 40;
volatile int8_t rotaryEncoder = 0; // the value of the rotary encoder, fated to be consumed into a chosen variable


#define HM_PLAY_BEAT // activates the LedMetronome
#ifdef HM_PLAY_BEAT
// pwm pin for the LedMetronome
static int pulseLed = 13;
#endif

// you MUST define this float once as a header request it
float LA4_REF = 440.0;

void interruptLowCHA() {
  static unsigned long lastTime = 0;
  static unsigned long thisTime = 0; // The sole reason this one is static is to avoid heap allocation and have a shorter interrupt
  // a tutorial that really works https://www.youtube.com/watch?v=J9cDEef0IbQ
  // Thank you Ralph S Bacon https://www.youtube.com/RalphBacon
  thisTime = millis();
  if (thisTime - lastTime > 5) {
    if (digitalRead(CHB)) // only read CHB because CHA is interrupting
      ++rotaryEncoder;
    else
      --rotaryEncoder;

    lastTime = thisTime;
  }
}



// TODO have a scale class interface and implementation, that will allow generator, and customized methods (currently we only support diatonic scales)
struct scaleDeprecated GAMME_Do = {0, 0, NOTE_DO, "Do majeur"};

// fa est #, note de base = mi
struct scaleDeprecated miMineur = {B1000, 0, NOTE_MI, "mi mineur"};

// fa est diese
struct scaleDeprecated solMajeur = {B1000, 0, NOTE_SOL, "sol majeur"};


struct scaleDeprecated miMajeur = {B11011, 0, NOTE_MI, "mi majeur"};

// E minor scale, 120 bpm (unit = halfbeat), 3/2 time signature
// gamme de mi mineur, 120 bpm (unité de battement = blanche), rythme = 3/2 (3 temps par mesure, unité dde temps = blanche)
// TODO have a class for this, maybe the calling code will be easier.
struct sheet tourdion = {&miMineur, 120, 48, 3, 2};

PlayingContext * pc = new PlayingContext(&tourdion);

// set up the 3 voices to play nothing for now (which hooks to play will be decided later)
Player *p1,*p2,*p3;

#ifdef HM_PLAY_BEAT
Player *metronome = new LedMetronome(pc, pulseLed);
#endif

// playing a list of note on an already created player is now as easy as this
void dummyPlay(Player *player, Playable *playable) {
  player->setVoice(playable);           // setting the voice in the plager
  while (!player->hasFinished()) {      // while the player has not finished playing
    player->playIfReady(millis());      // call this method with the millisecond as parameter
  }
}

// first we are defining the raw notes to be used in the hooks
Playable *blanche = new Note(48);             //half (en) blanche (fr)
Playable *noire = new Note(24);               //quarter (en) noire (fr)
Playable *croche = new Note(12);              //eight's (en) croche (fr)
Playable *blanchePointee = new Note(72);      //dottedHalf (en) blanche pointée (fr)
Playable *ronde = new Note(96);               //full (en) ronde (fr)
Playable *rondePointee = new Note(144);       //dottedFull (en) ronde pointée (fr)
Playable *fullCase = new Note(tourdion.top * tourdion.bottom * 24); // a note during the full case (en) une représentation du temps de la mesure entière (fr)
/**
   (en) A hook is a list of not to play with offset with respect to the hook containing them
        In this example, we play 1st note at the height of the hook, then up one note, then up one note again, then down one note, then down one note,
        all durations are quarter notes.

   (fr) Un hook est une suite de notes avec des décalages par rapport à la référence passée par le hook les référençant.
        Ici , on joue une note à la hauteur désirée, on monte d'une note, puis d'une autre, on redescend d'une note, et d'une autre, sur des noires.
*/
Playable *hook1_1 = (new ListHook(6))->add(noire)
                    ->add(noire, 1)
                    ->add(noire, 2)
                    ->add(noire, 3)
                    ->add(noire, 2)
                    ->add(noire, 1);
/**
   (en) You can also go under the hook reference

   (fr) On peut descendre aussi sous la reference du hook
*/
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
/**
   (en) Instead of a note, a hook can call another hook.
        Here we show how to reuse hooks inside hooks:
        - call the previously defined hook hook1_1 at the base level, (its content will be played recursively)
        - play a dotted half note at the base level
        - play again the same hook one note upper on the scale (each element of this hook will be offset on the scale)
        - play another hook, hook1_2, yet anothe note upper,
        - play again the 1st hook at the base level,
        - play a third hook

   (fr) Au lieu d'une note, un hook peut contenir un autre hook
       Pour ce hook, voici comment réutiliser des hooks à l'intérieur de hooks, ainsi on va
       - appeler le hook hook1_1 défini précédement au niveau de base (son contenu va être joué par récursion)
       - jouer une blanche pointée au niveau de base
       - rejouer encore ce hook un note au dessus sur la gamme (chaque note ou sous-hook de ce hook vont être décalés sur la gamme)
       - jouer un autre hook, hook1_2, encore une note au dessus
       - jouer une nouvelle fois le premier hook au niveau de base
       - jouer un 3ème hook
*/
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
/**
   (en) You can have accidents, whether it will have an impact will be calculated against the scale.
        For example forcing a sharp on an already sharp note will not have any effect.
        If you force a flat to a sharp note on the scale it will be lowered by 2 half tones.

   (fr) On gère les accidents correctement: si on force dièse sur une note qui est déjà un dièse ça n'aura pas d'effet.
        Si on force un bémol sur une note qui est sensée être dièse sur la gamme elle sera baissée de 2 demi-tons.
*/
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
                    ->add(blanche, -4);
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

Playable *b_Hook1_1 = (new ListHook(2))->add(ronde)
                      ->add(blanche);
Playable *c_Hook1_1 = (new ListHook(2))->add(ronde)
                      ->add(blanche, -2);
Playable *c_Hook1_2 = (new ListHook(2))->add(ronde)
                      ->add(blanche,  1);
Playable *c_Hook1_3 = (new ListHook(2))->add(ronde)
                      ->add(blanche, -1);
Playable *c_Hook1_4 = (new ListHook(5))->add(blanche)
                      ->add(noire, -1)
                      ->add(noire, -2)
                      ->add(blanche, -1)
                      ->add(rondePointee, -2);
Playable *c_Hook2_5 = (new ListHook(3))->add(blanche)->add(ronde, -1)->add(rondePointee, -2);
Playable *c_firstPhrase = (new ListHook(7))->add(c_Hook1_1, 2)
                          ->add(b_Hook1_1, 4)
                          ->add(c_Hook1_2, 4)
                          ->add(c_Hook1_3, 4)
                          ->add(b_Hook1_1, 4)
                          ->add(c_Hook1_2, 2)
                          ->add(c_Hook1_4, 2);
Playable *c_secondPhrase = (new ListHook(7))->add(b_Hook1_1)
                           ->add(new RepeatHook(b_Hook1_1, 2), -1)
                           ->add(new RepeatHook(b_Hook1_1, 2), -3)
                           ->add(c_Hook1_3, -3)
                           ->add(c_Hook2_5, -5);
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

/**
   (en) A RepeatHook is a hook that repeats the parameterized hook a given number of times, or infinitely if the number is omitted or set to 0

   (fr) Un RepeatHook est un hook qui permet de répéter le hook en paramètre un nombre choisi de fois, ou indéfiniment si ce nombre est 0 ou omis
*/
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

Playable *fullSoprano = (new ListHook(3))->add(new RepeatHook(c_firstPhrase, 2), 2)
                        ->add(new RepeatHook(c_secondPhrase, 2), 7);
Playable *fullAlto = (new ListHook(3))->add(new RepeatHook(firstPhrase, 2))
                     ->add(new RepeatHook(secondPhrase, 2), 4);
Playable *fullHommes = (new ListHook(3))->add(new RepeatHook(b_firstPhrase, 2))
                       ->add(new RepeatHook(b_secondPhrase, 2));
// TODO show we can call unuse and explain how to free memory
#ifdef HM_PLAY_BEAT
Playable *beat; // beat is set in setup()
#endif

// here are the top hooks. We will play each of them start with silence during 2 fullCases in order to show that the led metronome playing before we can hear sound.
Playable *tourdionVoice1 = (new ListHook(6))->add(new RepeatHook(fullCase, 2), 0, NOTE_IS_SILENCE)
                           ->add(fullAlto)
                           ->add(fullCase, 0, NOTE_IS_SILENCE);
Playable *tourdionVoice2 = (new ListHook(5))->add(new RepeatHook(fullCase, 2), 0, NOTE_IS_SILENCE)
                           ->add(fullHommes);
Playable *tourdionVoice3 = (new ListHook(5))->add(new RepeatHook(fullCase, 2), 0, NOTE_IS_SILENCE)
                           ->add(fullSoprano);

int state = 0; // playing
void setup() {
  // we can create a player without setting what to play now on 1st player, we can use THE_NOTHING which will play nothing if the player is called
  p1 = new TonePlayer(pc, voicePin1, THE_NOTHING);
  p2 = new TonePlayer(pc, voicePin2, tourdionVoice2);
  p3 = new TonePlayer(pc, voicePin3, tourdionVoice3);

  pinMode(stateLed, OUTPUT);
  pinMode(CHA, INPUT_PULLUP);
  pinMode(CHB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CHA), interruptLowCHA, LOW);
  Serial.begin(9600);
// test Scale constructor
  Scale *s1 = new DiatonicScale("3#M");
  Scale *s2 = new DiatonicScale("3#m");
  Scale *s3 = new DiatonicScale("2bM");
  Scale *s4 = new DiatonicScale("2bm");
  Serial.println("Scales:");
  Serial.println(s1->getName());
  Serial.println(s2->getName());
  Serial.println(s3->getName());
  Serial.println(s4->getName());
  // play some jingle in the set scale
  // the jingle will be one octave lower using this trick (to show wa can interact with the pitch on real time)
  LA4_REF = 220.0;
  ListHook *suite = new ListHook(6);
  suite->add(blanche, -2);          // one half note
  for (int i = 1; i <  5; ++i)      // 4 more notes, on the scale, they are eight's note
    suite->add(croche, i - 2);
  suite->add(blanche, 3);           // one half note
  dummyPlay(p1, suite);             // this changes the voice played on Player p1
  // go back to the normal pitch
  LA4_REF = 440.0;
  delay(2000);
  digitalWrite(stateLed, HIGH);
  p1->setVoice(tourdionVoice1);
}
void(* resetFunc) (void) = 0;
void loop() {
  if (state == 0) {
    if (rotaryEncoder != 0) {
      Serial.print("+");
      Serial.print(rotaryEncoder);
      Serial.print("\t");
      Serial.print(LA4_REF);
      Serial.print(" -> ");
      cli();
      LA4_REF += rotaryEncoder;
      rotaryEncoder = 0;
      sei();
      Serial.println(LA4_REF);
    }
    unsigned long currentMillis = millis();

    if (p1->hasFinished()) {
      state = 1;
      digitalWrite(stateLed, LOW);
#ifdef HM_PLAY_BEAT
      digitalWrite(pulseLed, LOW);
#endif
      set_sleep_mode (SLEEP_MODE_PWR_DOWN);
      sleep_enable();
      sleep_cpu();
    }
    p1->playIfReady(currentMillis);
    p2->playIfReady(currentMillis);
    p3->playIfReady(currentMillis);

    // simulate that other calculus take time to show the interupt is robust
    // warning: desynchronizes notes ! Synchronization hook is not implemented yet
    //delay(20);
#ifdef HM_PLAY_BEAT
    metronome->playIfReady(currentMillis);
#endif
  }
}
