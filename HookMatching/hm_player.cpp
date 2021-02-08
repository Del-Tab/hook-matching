/*
   please keep this reference in this file when using this code anywhere
   https://github.com/DelTa-B/hook-matching/
   I would be glad if you give this link when you take part of this file :)
*/
#include "hm_player.hpp"

//////////////////////////
// note Hook            //
//////////////////////////
boolean note::hasMore(uint8_t *hc, uint8_t depth) {
  return hc[depth] == 0;
}

struct note_info note::getOne(uint8_t *hc, uint8_t depth) {
  struct note_info ret = { 0, 0, duration};
  ++(hc[depth]);
  return ret;
}

uint8_t note::getMaxDepth() {
  return 1;
}


playing_context::playing_context(struct sheet_dep *a_sheetInfo, scale *a_scale) : sheetInfo(a_sheetInfo), scaleInfo(a_scale != NULL ? a_scale : a_sheetInfo->default_scale) { };
struct sheet_dep *playing_context::getSheetInfo() {
  return sheetInfo;
}
float playing_context::get_frequency(const struct note_info *ni) {
  return scaleInfo->get_frequency(ni);
}

uint32_t playing_context::getDurationMillis(const struct note_info *ni) {
  return getNoteLengthMillis(ni->duration, sheetInfo);
}



//////////////////////////
// playable             //
//////////////////////////
// linked to the memory usage:
playable::playable(): nb_usage(1) {}; // we suppose it's used once when allocated . call unuse() to free
playable *playable::useAgain() {
  ++nb_usage;
  return this;
}
void playable::unuse() {
  if (--nb_usage == 0) delete this;
}

//////////////////////////
// player               //
//////////////////////////
boolean player::isReady(unsigned long millis) {
  return nextTime <= millis;
}
player::player(playing_context *a_pc, playable *a_voice) : pc(a_pc), voice(a_voice->useAgain()), coordinates{0}, nextTime(0) { }
boolean player::hasFinished() {
  return !voice->hasMore(coordinates, 0) && isReady(millis());
}
void player::setVoice(playable *n_voice) {
  if (voice != NULL)
    voice->unuse();
  voice = n_voice->useAgain();
  // reset coordinates
  memset(coordinates, 0, sizeof coordinates);
}



//////////////////////////
// Repeat Hook          //
//////////////////////////
repeat_hook::repeat_hook(playable *a_p, uint8_t a_nb_cycles) : p(a_p->useAgain()), nb_cycles(a_nb_cycles) { };
repeat_hook::~repeat_hook() {
  p->unuse();
}

boolean repeat_hook::hasMore(uint8_t *hc, uint8_t depth) {
  if (nb_cycles == 0)
    return true;
  if (hc[depth] >= nb_cycles)
    return false;
  return p->hasMore(hc, depth + 1);
}

struct note_info repeat_hook::getOne(uint8_t *hc, uint8_t depth) {
  struct note_info childOne = p->getOne(hc, depth + 1);
  if (!p->hasMore(hc, depth + 1)) {
    hc[depth + 1] = 0;
    ++(hc[depth]);
  }
  return childOne;
}

uint8_t repeat_hook::getMaxDepth() {
  return p->getMaxDepth() + 1;
}

//////////////////////////
// List Hook            //
//////////////////////////
list_hook::list_hook(uint8_t a_capacity) : capacity(a_capacity), number(0), maxDepth(0) {
  list = (struct PlayableChild *)malloc(a_capacity * sizeof(struct PlayableChild));
  if (list == NULL)
    Serial.println("malloc error");
}
list_hook::~list_hook() {
  if (list != NULL) {
    for (int i = 0; i < number; ++i)
      list[i].p->unuse();
    free(list);
  }
}
list_hook *list_hook::add(playable *p, hm_offset degreeOffset, effects flags) {
  list[number++] = {degreeOffset, flags, p->useAgain()};
  maxDepth = max(maxDepth, p->getMaxDepth() + 1);
  return this;
}
boolean list_hook::hasMore(uint8_t *hc, uint8_t depth) {
  if (hc[depth] < number - 1)
    return true;
  if (hc[depth] >= number)
    return false;
  list[hc[depth]].p->hasMore(hc, depth + 1);
}
struct note_info list_hook::getOne(uint8_t *hc, uint8_t depth) {
  struct PlayableChild pc = list[hc[depth]];
  note_info ret = pc.p->getOne(hc, depth + 1);
  if (!pc.p->hasMore(hc, depth + 1)) {
    hc[depth + 1] = 0;
    ++(hc[depth]);
  }
  ret.flags |= pc.flags;
  // correction with the current structure's offset :)
  ret.degreeOffset += pc.degreeOffset;
  return ret;
}
uint8_t list_hook::getMaxDepth() {
  return maxDepth;
}
//////////////////////////
// led metronome        //
//////////////////////////
led_metronome::led_metronome(playing_context *_pc, uint8_t _pulseLed) : player(_pc, THE_NOTHING), pulseLed(_pulseLed), brightness(0) {
  pinMode(pulseLed, OUTPUT);
  uint8_t beatNoteDuration =  24 * 4 / pc->getSheetInfo()->bottom;
  playable *beatNote = new note(beatNoteDuration);
  list_hook *oneMeasureBeat =  new list_hook(pc->getSheetInfo()->top);
  for (int i = 0; i <  pc->getSheetInfo()->top; ++i) {
    if (i == 0)
      oneMeasureBeat->add(beatNote, 15);
    else if (i * 2 == pc->getSheetInfo()->top)
      oneMeasureBeat->add(beatNote, 6);
    else
      oneMeasureBeat->add(beatNote, 2);
  }
  setVoice(new repeat_hook(oneMeasureBeat));
  oneMeasureBeat->unuse();
}

void led_metronome::playIfReady(unsigned long currentMillis) {
  // fade by step
  if (brightness > 0) {
    brightness -= HM_PULSE_STEP;
    if (brightness < 0)
      brightness = 0;
    analogWrite(pulseLed, brightness);
  }
  if (this->isReady(currentMillis)) {
    note_info ni = voice->getOne(coordinates, 0);
    uint32_t dur = pc->getDurationMillis(&ni);
    brightness = map((uint8_t) ni.degreeOffset, 0, 15, 0, 255);
    analogWrite(pulseLed, brightness);
    nextTime = currentMillis + dur;
  }
}

//////////////////////////
// Dummy playable       //
//////////////////////////
boolean nothing::hasMore(uint8_t *hc, uint8_t depth) {
  return false;
}
struct note_info nothing::getOne(uint8_t *hc, uint8_t depth) {
  return {};   // thou shall not call getOne() when hasMore returned false
}
uint8_t nothing::getMaxDepth() {
  return 0;
}


//////////////////////////
// a tone player        //
//////////////////////////
tone_player::tone_player(playing_context *a_pc, uint8_t a_tonePin, playable *a_voice) : player(a_pc, a_voice), toneVoice(new Tone()) {
  pinMode(a_tonePin, OUTPUT);
  toneVoice->begin(a_tonePin);
};
void tone_player::playIfReady(unsigned long currentMillis) {
  if (isReady(currentMillis)) {
    if (voice->hasMore(coordinates, 0)) {
      note_info ni = voice->getOne(coordinates, 0);
      float freq = pc->get_frequency(&ni);
      uint32_t dur = pc->getDurationMillis(&ni);

      if ((ni.flags & NOTE_IS_SILENCE) == 0)
        toneVoice->play(round(freq), .955 * dur);

      // we take into accound this loop's calculus time
      nextTime = currentMillis + dur;
    }
  }
}
