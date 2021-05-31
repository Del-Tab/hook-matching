/*
   please keep this reference in this file when using this code anywhere
   https://github.com/DelTa-B/hook-matching/
   I would be glad if you give this link when you take part of this file :)
*/
#ifndef HM_DEFINITONS_HPP
#define HM_DEFINITONS_HPP

#include <Arduino.h> // we need the types
#include <avr/pgmspace.h>

#ifndef HM_DAC_PLAYER_CS
# define HM_DAC_PLAYER_CS 53
#endif
#ifndef HM_SAMPLE_FREQUENCY
//225 * referenceFrequency | si8=3951
// that means that frequencies above referenceFrequency will use less samples
# define HM_SAMPLE_FREQUENCY 1011473L// 11025
#endif
#ifndef HM_TIMER_PRESCALER
# define HM_TIMER_PRESCALER 1
#endif

// the higher the resolution, the lower the number of voices to fit in 16bits
using dac_data = uint16_t;
#ifndef HM_DAC_RESOLUTION
# define HM_DAC_RESOLUTION 12
#endif
#ifndef HM_MAX_VOICE
#define HM_MAX_VOICE 16
#endif


using effects = uint8_t;
#define NOTE_IS_SILENCE         B00000001
#define NOTE_STICKS_TO_NEXT     B00000010
#define NOTE_FORCE_SHARP        B00000100
#define NOTE_FORCE_FLAT         B00001000
#define NOTE_FORCE_NATURAL      B00010000
#define NOTE_FORCE_DOUBLE_SHARP B00100000
#define NOTE_FORCE_DOUBLE_FLAT  B01000000

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

extern float LA4_REF; // the frequency of LA (most of the time 440.0 hz)

using note_duration = uint8_t;  // 24 for a black, 48 for a white, handles triolets
using note_t = uint8_t;         // 0 for C, 1 for D, etc.
using hm_offset = int8_t;       // relative note/octave position

struct note_info {
  hm_offset degreeOffset;
  hm_offset octaveOffset;
  note_duration duration;
  effects flags;
};


//typedef int (*PLAY_CB)(void * context, float freq, uint32_t duration);

#endif // HM_DEFINITONS_HPP
