#ifndef HM_MUSIC_HPP
#define HM_MUSIC_HPP

#include "hm_definitions.hpp"
#include "hm_scale.hpp"

// TODO ref2 put it in a sheet class
struct sheet_dep {
  scale *default_scale;
  uint16_t bpm;           // most of the time ranges from 96 to 480, but you're free, just don't chose 0, and consider we are millisecond precise only
  note_duration bpm_unit; // 24 when bpm is expressed on the quarter note, 48 on the half note, etc
  uint8_t top;            // number of time unit per case
  note_duration bottom;   // if it's 4, quarter notes are one time unit, if it's 2, half notes are 1 time unit, if it's 8 eights are 1 time unit etc
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

uint32_t getNoteLengthMillis(note_duration nd, const struct sheet_dep & p);

#endif // HM_MUSIC_HPP
