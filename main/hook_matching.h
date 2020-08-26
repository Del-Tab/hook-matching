#ifndef HOOK_MATCHING_H
#define HOOK_MATCHING_H

/* 
 * please keep this reference in this file when using this code anywhere 
 * https://github.com/DelTa-B/hook-matching/
 */

#define NOTE_LINK_LEFT 1
#define NOTE_LINK_RIGHT 2

#define NOTE_DO 0
#define NOTE_RE 1
#define NOTE_MI 2
#define NOTE_FA 3
#define NOTE_SOL 4
#define NOTE_LA 5
#define NOTE_SI 6

#define NOTE_C NOTE_DO
#define NOTE_D NOTE_RE
#define NOTE_E NOTE_MI
#define NOTE_F NOTE_FA
#define NOTE_G NOTE_SOL
#define NOTE_A NOTE_LA
#define NOTE_B NOTE_SI


#define NOTE_TONIQUE 0
#define NOTE_SECONDE 1
#define NOTE_TIERCE 2
#define NOTE_QUARTE 3
#define NOTE_SOUS_DOMINANTE NOTE_QUARTE
#define NOTE_QUINTE 4
#define NOTE_DOMINANTE NOTE_QUINTE
#define NOTE_SIXTE 5
#define NOTE_SEPTIEME 6
#define NOTE_SENSIBLE NOTE_SEPTIEME
#define NOTE_OCTAVE 7;

#define FORCE_SHARP 1
#define FORCE_FLAT 2
#define FORCE_NATURAL 4

static float LA4_REF = 440.0;
typedef uint8_t note_duration;  // 24 for a black, 48 for a white, handles triolets
typedef uint8_t note;           // 0 for C, 1 for D, etc.
/* 
 * modulo that handles correctly negative numbers (only returns positives values)
 * Not tested on compiler that already handle florring of negative numbers correctly but I guess it shall work too ...
 */
int hm_mod(int a, int b) 
{
  if (b < 0)
    return -hm_mod(-a, -b);
  int ret = a % b;
  if (ret < 0)
    ret += b;
  return ret;
}
/* 
 * division that handle correctly negative number (floors to the greatest int lower than or equals to the real value) 
 * We check the compiler dependent modulo behavior to guess the floring of the corresponding division 
 * Not tested on compiler that already handle florring of negative numbers correctly but I guess it shall work too ...
 */
int hm_div(int a, int b)
{
  float ret = 1. * a / b;
  if ((a%b) < 0) 
    return (int)ret - 1;
  return (int)ret;
}

/** scale is a description of the rule on which the note walk. 
 * Definition is made taking the C major scale as reference, which is what you get by only playing white notes on a piano 
 */
const uint8_t diatonic_offsets[] = {0, 2, 4, 5, 7, 9, 11};
const uint8_t scaleSize = sizeof diatonic_offsets/sizeof diatonic_offsets[0];
struct scale {
  uint8_t sharps;           // binary map from right to left, starting from do: 1 if sharp
  uint8_t flats;            // binary map from right to left, starting from do: 1 if flat
  note note_base;        // the NOTE_something telling which note is the Tonic
  char *display_name;    // a fancy name to display
};


struct hookpart {
  int8_t degreeOffset;
  note_duration duration;
};

struct scale_hook {
  size_t number;
  struct hookpart part [];
};

struct contextual_scale_hook {
  
  uint8_t octave;
  uint8_t note;
  struct scale_hook *hook;
};
float getFrequency(const int8_t degree, const uint8_t octave,  struct scale const *g);


struct sheet {
  struct scale * scale;
  uint16_t bpm; // may ranges from 96 to 480
  note_duration bpm_unit;
  uint8_t top;    // number of time unit per "mesure"
  uint8_t bottom; // if it's 4, blacks are one time unit, if it's 2, white ahre 1 time unit, if it's 8 "croches" are 1 time unit etc
};
uint32_t getNoteLengthMillis(note_duration nd, const struct sheet & p) {
  float base_duration = 60000.0 / (p.bpm_unit * p.bpm);
  return nd * base_duration;
}

void play (const int pin, const struct contextual_scale_hook & partie, const struct sheet & p, const struct scale *g) {
  for (int i = 0; i < partie.hook->number; ++i) {
    float freq = getFrequency(partie.note - g->note_base + partie.hook->part[i].degreeOffset, partie.octave, g);
    uint32_t dur = getNoteLengthMillis(partie.hook->part[i].duration, p);
    tone(pin, round(freq), dur - 25);
    delay(dur);
  }
}
void play (const int pin, const struct contextual_scale_hook & partie, const struct sheet & p) {
  play (pin, partie, p, p.scale);
}

/* return true if the degree in the map is sharp */
bool isSharp (const struct scale *g, byte degre) {
  return (1 << hm_mod(degre + g->note_base,7)) & g->sharps;
}

/* return true if the degree in the map is flat */
bool isFlat (const struct scale *g, byte degre) {
  return (1 << hm_mod(degre + g->note_base,7)) & g->flats;
}

/* return true if the degree in the map is neither sharp nor flat */
bool isNatural (const struct scale *g, byte degre) {
  return !(isSharp(g,degre) ^ isFlat(g,degre));
}




uint16_t getMidiNote(const int8_t degree, const uint8_t octave, const struct scale * g) {
  uint16_t ret = (octave  + hm_div((degree + g->note_base), scaleSize)) * 12 
         + diatonic_offsets[hm_mod((hm_mod(degree, scaleSize)+ g->note_base), scaleSize)];
  if (isSharp(g, degree))
    ++ret;
  if (isFlat(g, degree))
    --ret;

  return ret;
}


/**
 * Gets frequencies for a diaonic-like scale
 * to be precise (albeit not exact) we work in floats, caller can round it to int if needed 
  
  round(getFrequency(NOTE_SIXTE, 5, GAMME_Do, 0)) must return LA4_REF
*/
float getFrequency(const int8_t degree, const uint8_t octave, struct scale const *g, const int8_t transpose) {
  // Getting base do (C) from current octave ... 
  int16_t offsetFromLa440 = getMidiNote(degree, octave, g) + transpose;
  offsetFromLa440 -= 5*12 + diatonic_offsets[NOTE_SIXTE];
  // getting the right frequency from the ref and offset
  float freq = LA4_REF * pow(2.0, offsetFromLa440/12.0);
  
  return freq;
}



float getFrequency(int8_t degree, uint8_t octave,  struct scale const *g) {
  return getFrequency(degree, octave, g, 0);
}
uint8_t getSensible(const struct scale *scale) {
  return hm_mod(scale->note_base - 1, 7);
}

uint8_t getDominante(const struct scale *scale) {
  return hm_mod(scale->note_base + 4, 7);
}

#endif
