#ifndef HM_DEFINITONS_HPP
#define HM_DEFINITONS_HPP
#include <Tone.h>

/*
   please keep this reference in this file when using this code anywhere
   https://github.com/DelTa-B/hook-matching/
   I would be glad if you give this link when you take part of this file :)
*/

using effects = uint8_t;
#define NOTE_IS_SILENCE 1
#define NOTE_STICKS_TO_NEXT 2
#define NOTE_FORCE_SHARP 4
#define NOTE_FORCE_FLAT 8
#define NOTE_FORCE_NATURAL 16

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

extern float LA4_REF;

using note_duration = uint8_t;  // 24 for a black, 48 for a white, handles triolets
using note_t = uint8_t;         // 0 for C, 1 for D, etc.


struct note_info {
  int8_t degreeOffset;
  uint8_t octaveOffset;
  note_duration duration;
  effects flags;
};


//typedef int (*PLAY_CB)(void * context, float freq, uint32_t duration);

#endif // HM_DEFINITONS_HPP
