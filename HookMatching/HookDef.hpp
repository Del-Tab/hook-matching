#ifndef HM_HOOKDEF_HPP
#define HM_HOOKDEF_HPP
#include <Arduino.h>

#include "hm_maths.hpp"
#include "hm_scale.hpp"

#ifndef MAX_DEPTH
# define MAX_DEPTH 16
#endif
#ifndef HM_PULSE_STEP
# define HM_PULSE_STEP .075
#endif



class PlayingContext {
  private:
    struct sheet_dep *sheetInfo;
    Scale *scaleInfo;
  public:
    PlayingContext(struct sheet_dep *_sheetInfo) : sheetInfo(_sheetInfo), scaleInfo(_sheetInfo->default_scale) { };
    struct sheet_dep *getSheetInfo() {
      return sheetInfo;
    }
    float get_frequency(struct note_info ni) {
      return scaleInfo->get_frequency(ni);
    }

    uint32_t getDurationMillis(struct note_info ni) {
      return getNoteLengthMillis(ni.duration, *sheetInfo);
    }

};

class Playable {
  private:
    int nb_usage;
  public:
    //virtual int getIteratorInitCtx() = 0;
    virtual boolean hasMore(uint8_t *hc, uint8_t maxDepth, uint8_t depth) = 0;
    virtual struct note_info getOne(uint8_t *hc, uint8_t maxDepth, uint8_t depth)  = 0;
    virtual uint8_t getMaxDepth() = 0;

    // linked to the memory usage:
    Playable(): nb_usage(1) {}; // we suppose it's used once when allocated . call unuse to free
    Playable *useAgain() {
      ++nb_usage;
      return this;
    }
    void unuse() {
      if (--nb_usage == 0) delete this;
    }
};

class Player {
  protected:
    PlayingContext *pc;
    Playable *voice;
    uint8_t coordinates[MAX_DEPTH];
    unsigned long nextTime;
    boolean isReady(unsigned long millis) {
      return nextTime <= millis;
    }
  public:
    Player(PlayingContext *a_pc, Playable *a_voice) : pc(a_pc), voice(a_voice->useAgain()), coordinates{0}, nextTime(0) { }
    boolean hasFinished() {
      return !voice->hasMore(coordinates, MAX_DEPTH, 0) && isReady(millis());
    }
    void setVoice(Playable *n_voice) {
      if (voice != NULL)
        voice->unuse();
      voice = n_voice->useAgain();
      // reset coordinates
      memset(coordinates, 0, sizeof coordinates);
    }
    virtual void playIfReady(unsigned long currentMillis) = 0;

};


/*

*/
class TonePlayer : public Player {
  private :
    Tone *toneVoice;
  public:
    TonePlayer(PlayingContext *a_pc, uint8_t a_tonePin, Playable *a_voice) : Player(a_pc, a_voice), toneVoice(new Tone()) {
      toneVoice->begin(a_tonePin);
    };
    void playIfReady(unsigned long currentMillis) {
      if (isReady(currentMillis)) {
        if (voice->hasMore(coordinates, MAX_DEPTH, 0)) {
          note_info ni = voice->getOne(coordinates, MAX_DEPTH, 0);
          float freq = pc->get_frequency(ni);
          uint32_t dur = pc->getDurationMillis(ni);

          if ((ni.flags & NOTE_IS_SILENCE) == 0)
            toneVoice->play(round(freq), .955 * dur);

          // we take into accound this loop's calculus time
          nextTime = currentMillis + dur;
        }
      }
    }

};




/**
   the null playable, play nothing at all (used when a voice is not initialized and you want to call methods without crashing)
*/
class Nothing : public Playable {
  public:
    Nothing() { }
    boolean hasMore(uint8_t *hc, uint8_t maxDepth, uint8_t depth) {
      return false;
    }
    struct note_info getOne(uint8_t *hc, uint8_t maxDepth, uint8_t depth) {
      return {};   // thou shall not call getOne() when hasMore returned false
    }
    uint8_t getMaxDepth() {
      return 0;
    }
};

static Playable *THE_NOTHING = new Nothing();



/**
   The simplest Playable: one single playable stuff that defines a duration, callable with offset and octave
*/
class Note : public Playable {
  private:
    note_duration duration;

  public:
    Note(note_duration a_duration): duration(a_duration) { }
    boolean hasMore(uint8_t *hc, uint8_t maxDepth, uint8_t depth);
    struct note_info getOne(uint8_t *hc, uint8_t maxDepth, uint8_t depth);
    uint8_t getMaxDepth();
};



/**
   A hook that repeats the inner Hook N times, or every time if N is 0
*/
class RepeatHook : public Playable {
  private:
    Playable *p;
    int nb_cycles;
    ~RepeatHook();
  public:
    RepeatHook(Playable *a_p, int a_nb_cycles = 0) : p(a_p->useAgain()), nb_cycles(a_nb_cycles) { }

    boolean hasMore(uint8_t *hc, uint8_t maxDepth, uint8_t depth);
    struct note_info getOne(uint8_t *hc, uint8_t maxDepth, uint8_t depth);
    uint8_t getMaxDepth();
};

/**
   A hook that is a list of PlayableChild_s with no relation whatsoever.They are played one after the other
*/
class ListHook : public Playable {
  private:
    struct PlayableChild {
      int8_t degreeOffset;
      effects flags;
      Playable *p;
    };
    uint8_t capacity; // max 255, todo do better with capacity, maybe use a linked list
    uint8_t number;
    uint8_t maxDepth;
    struct PlayableChild *list;
    ~ListHook();
  public:
    ListHook(uint16_t a_capacity);

    ListHook *add(Playable *p, int8_t degreeOffset = 0, effects flags = 0);
    boolean hasMore(uint8_t *hc, uint8_t maxDepth, uint8_t depth);
    struct note_info getOne(uint8_t *hc, uint8_t maxDepth, uint8_t depth);
    uint8_t getMaxDepth();
};


class LedMetronome : public Player {
  private:
    unsigned int pulseLed;
    int brightness;
  public:
    LedMetronome(PlayingContext *_pc, unsigned int _pulseLed) : Player(_pc, THE_NOTHING), pulseLed(_pulseLed), brightness(0) {
      pinMode(pulseLed, OUTPUT);
      uint8_t beatNoteDuration =  24 * 4 / pc->getSheetInfo()->bottom;
      Playable *beatNote = new Note(beatNoteDuration);
      ListHook *oneMeasureBeat =  new ListHook(pc->getSheetInfo()->top);
      for (int i = 0; i <  pc->getSheetInfo()->top; ++i) {
        if (i == 0)
          oneMeasureBeat->add(beatNote, 15);
        else if (i * 2 == pc->getSheetInfo()->top)
          oneMeasureBeat->add(beatNote, 6);
        else
          oneMeasureBeat->add(beatNote, 2);
      }
      setVoice(new RepeatHook(oneMeasureBeat));
      oneMeasureBeat->unuse();
    }
    void playIfReady(unsigned long currentMillis) {
      // fade by step
      if (brightness > 0) {
        brightness -= HM_PULSE_STEP;
        if (brightness < 0)
          brightness = 0;
        analogWrite(pulseLed, brightness);
      }
      if (this->isReady(currentMillis)) {
        note_info ni = voice->getOne(coordinates, MAX_DEPTH, 0);
        uint32_t dur = pc->getDurationMillis(ni);
        brightness = map((uint8_t) ni.degreeOffset, 0, 15, 0, 255);
        analogWrite(pulseLed, brightness);
        nextTime = currentMillis + dur;
      }
    }
};
#endif // HM_HOOKDEF_HPP
