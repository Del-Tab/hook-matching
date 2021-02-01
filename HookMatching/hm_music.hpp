#ifndef HM_MUSIC_HPP
#define HM_MUSIC_HPP

#include "hm_definitions.hpp"
#include "hm_scale.hpp"
struct sheet {
  // TODO ref1
  //struct scaleDeprecated * default_scale;
  Scale *default_scale;
  uint16_t bpm; // most of the time ranges from 96 to 480, just don't chose 0
  note_duration bpm_unit; //24 when bpm is expressed on the quarter note, 48 on the half note
  uint8_t top;    // number of time unit per "mesure"
  note_duration bottom; // if it's 4, blacks are one time unit, if it's 2, white ahre 1 time unit, if it's 8 "croches" are 1 time unit etc
};
/*
   modulo that handles correctly negative numbers (only returns positives values)
   Not tested on compiler that already handle florring of negative numbers correctly but I guess it shall work too ...
*/
int hm_mod(int a, int b);
/*
   division that handle correctly negative number (floors to the greatest int lower than or equals to the real value)
   We check the compiler dependent modulo behavior to guess the floring of the corresponding division
   Not tested on compiler that already handle florring of negative numbers correctly but I guess it shall work too ...
*/
int hm_div(int a, int b);

uint32_t getNoteLengthMillis(note_duration nd, const struct sheet & p);


// TODO ref1
/* return true if the degree in the map is sharp */
bool isSharp (const struct scaleDeprecated *g, byte degre);
// TODO ref1
/* return true if the degree in the map is flat */
bool isFlat (const struct scaleDeprecated *g, byte degre);
// TODO ref1
/* return true if the degree in the map is neither sharp nor flat */
bool isNatural (const struct scaleDeprecated *g, byte degre);

uint16_t getMidiNote(const int8_t degree, const uint8_t octave, const struct scaleDeprecated * g);

/**
   Gets frequencies for a diaonic-like scale
   to be precise (albeit not exact) we work in floats, caller can round it to int if needed

  contract for not being exact:
  when partial part of LA4_REF is not close to 0.5,
  round(getFrequency(NOTE_SIXTE, 5, GAMME_Do, 0)) must return round(LA4_REF)
*/
// TODO ref1 replace by a method in the "scale" class
float getFrequency(const int8_t degree, const uint8_t octave, struct scaleDeprecated const *g, const int8_t transpose);
float getFrequency(int8_t degree, uint8_t octave,  struct scaleDeprecated const *g) ;

// TODO ref1 replace by a method in the "scale" class
uint8_t getSensible(const struct scaleDeprecated *scale);
uint8_t getDominante(const struct scaleDeprecated *scale);
#endif // HM_MUSIC_HPP
