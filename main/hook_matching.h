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
    return (int)ret -1;
  return (int)ret;
}

/** scale is a description of the rule on which the note walk. 
 * Definition is made taking the C major scale as reference, which is what you get by only playing white notes on a piano 
 */
uint8_t diatonic_offsets[] = {0,2,4,5,7,9,11};
struct scale {
  byte sharps;           // binary map from right to left, starting from do: 1 if sharp
  byte flats;            // binary map from right to left, starting from do: 1 if flat
  byte note_base;        // the NOTE_something telling which note is the Tonic
  char *display_name;    // a fancy name to display
};
/**
  a sery of notes based on degree
  not used yet
*/
struct scale_hook {
  size_t number;
  uint8_t *degreeOffsets;
  int * durations;
  byte accidentSharps;
  byte accidentFlats;
  byte accidentNatural; 
};

/* return true if the degree in the map is sharp */
bool isSharp (struct scale g, byte degre) {
  return (1 << hm_mod(degre + g.note_base,7)) & g.sharps;
}

/* return true if the degree in the map is flat */
bool isFlat (struct scale g, byte degre) {
  return (1 << hm_mod(degre + g.note_base,7)) & g.flats;
}

/* return true if the degree in the map is neither sharp nor flat */
bool isNatural (struct scale g, byte degre) {
  return isSharp(g,degre) ^ isFlat(g,degre);
}



/**
 * Gets frequencies for a diaonic-like scale
 * to be precise (albeit not exact) we work in floats, caller can round it to int if needed 
  
  round(getFrequency(NOTE_SIXTE, 5, GAMME_Do, 0)) must return LA4_REF
*/
float getFrequency(int8_t degree, uint8_t octave,  struct scale g, int8_t transpose) {
  // Getting base do (C) from current octave ... 
  int16_t offsetFromLa440 = (octave - 5 + hm_div((degree + g.note_base),sizeof diatonic_offsets/sizeof diatonic_offsets[0])) * 12 - diatonic_offsets[NOTE_SIXTE] + transpose;
  // adding target note offset from scale base ...
  offsetFromLa440 += diatonic_offsets[hm_mod((hm_mod(degree,sizeof diatonic_offsets/sizeof diatonic_offsets[0])+ g.note_base),7)]; // we get real note
  // fixing sharpness :-) 
  if (isSharp(g, degree))
    ++offsetFromLa440;
  if (isFlat(g, degree))
    --offsetFromLa440;
  // getting the right frequency from the ref and offset
  float freq = LA4_REF * pow(2.0, offsetFromLa440/12.0);
  
  return freq;
}
float getFrequency(int8_t degree, uint8_t octave,  struct scale g) {
  return getFrequency(degree, octave, g, 0);
}
uint8_t getSensible(struct scale scale) {
  return hm_mod(scale.note_base - 1, 7);
}

uint8_t getDominante(struct scale scale) {
  return hm_mod(scale.note_base + 4, 7);
}

#endif
