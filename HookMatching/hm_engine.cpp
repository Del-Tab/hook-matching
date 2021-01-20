#include <Arduino.h>

#include "hm_music.hpp"

const uint8_t diatonic_offsets[] = {0, 2, 4, 5, 7, 9, 11};
const uint8_t scaleSize = sizeof diatonic_offsets/sizeof diatonic_offsets[0];


int hm_mod(int a, int b) 
{
  if (b < 0)
    return -hm_mod(-a, -b);
  int ret = a % b;
  if (ret < 0)
    ret += b;
  return ret;
}

int hm_div(int a, int b)
{
  float ret = 1. * a / b;
  if ((a%b) < 0) 
    return (int)ret - 1;
  return (int)ret;
}

int default_play_cb(void * context, float freq, uint32_t duration)
{
  struct default_play_context* ctx = (struct default_play_context*)context;
  ctx->voice.play(round(freq), duration - 20);
  delay(duration);  
  return 0;
}

void walk (PLAY_CB play_cb, void *context, const struct contextual_scale_hook & partie, const struct sheet & p, const struct scale *g) {
  for (int i = 0; i < partie.hook->number; ++i) {
    float freq = getFrequency(partie.note - g->note_base + partie.hook->part[i].degreeOffset, partie.octave, g);
    uint32_t dur = getNoteLengthMillis(partie.hook->part[i].duration, p);
    play_cb(context, freq, dur);
  }
}

void walk (PLAY_CB play_cb, void *context, const struct contextual_scale_hook & partie, const struct sheet & p) {
  walk (play_cb, context, partie, p, p.scale);
}

float getFrequency(const int8_t degree, const uint8_t octave, struct scale const *g, const int8_t transpose) {
  // Getting base do (C) from current octave ... 
  int16_t offsetFromLa440 = getMidiNote(degree, octave, g) + transpose;
  offsetFromLa440 -= 5*12 + diatonic_offsets[NOTE_SIXTE];
  // getting the right frequency from the ref and offset
  float freq = LA4_REF * pow(2.0, offsetFromLa440/12.0);
  
  return freq;
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

uint32_t getNoteLengthMillis(note_duration nd, const struct sheet & p) {
  float base_duration = 60000.0 / (p.bpm_unit * p.bpm);
  return nd * base_duration;
}

bool isSharp (const struct scale *g, byte degre) {
  return (1 << hm_mod(degre + g->note_base, 7)) & g->sharps;
}

bool isFlat (const struct scale *g, byte degre) {
  return (1 << hm_mod(degre + g->note_base,7)) & g->flats;
}

bool isNatural (const struct scale *g, byte degre) {
  return !(isSharp(g,degre) ^ isFlat(g,degre));
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
