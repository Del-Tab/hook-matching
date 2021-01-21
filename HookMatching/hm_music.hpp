#ifndef HOOK_MATCHING_HPP
#define HOOK_MATCHING_HPP

#include "hm_definitions.hpp"

/* 
 * modulo that handles correctly negative numbers (only returns positives values)
 * Not tested on compiler that already handle florring of negative numbers correctly but I guess it shall work too ...
 */
int hm_mod(int a, int b);
/* 
 * division that handle correctly negative number (floors to the greatest int lower than or equals to the real value) 
 * We check the compiler dependent modulo behavior to guess the floring of the corresponding division 
 * Not tested on compiler that already handle florring of negative numbers correctly but I guess it shall work too ...
 */
int hm_div(int a, int b);

uint32_t getNoteLengthMillis(note_duration nd, const struct sheet & p);

/* return true if the degree in the map is sharp */
bool isSharp (const struct scale *g, byte degre);

/* return true if the degree in the map is flat */
bool isFlat (const struct scale *g, byte degre);

/* return true if the degree in the map is neither sharp nor flat */
bool isNatural (const struct scale *g, byte degre);

uint16_t getMidiNote(const int8_t degree, const uint8_t octave, const struct scale * g);

/**
 * Gets frequencies for a diaonic-like scale
 * to be precise (albeit not exact) we work in floats, caller can round it to int if needed 
  
  contract for not being exact:
  when partial part of LA4_REF is not close to 0.5,
  round(getFrequency(NOTE_SIXTE, 5, GAMME_Do, 0)) must return round(LA4_REF)
*/
// TODO replace by a method in the "scale" class
float getFrequency(const int8_t degree, const uint8_t octave, struct scale const *g, const int8_t transpose);
float getFrequency(int8_t degree, uint8_t octave,  struct scale const *g) ;

// TODO replace by a method in the "scale" class
uint8_t getSensible(const struct scale *scale);
uint8_t getDominante(const struct scale *scale);
#endif
