#ifndef HM_MUSIC_HPP
#define HM_MUSIC_HPP

#include "hm_definitions.hpp"
#include "hm_scale.hpp"
struct sheet {
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

#endif // HM_MUSIC_HPP
