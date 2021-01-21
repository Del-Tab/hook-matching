
#include <Arduino.h>
#include "hm_definitions.hpp"
#include "hm_music.hpp"


struct note_info {
  int8_t degreeOffset;
  uint8_t octaveOffset;
  note_duration duration;
  effects flags;
};






class PlayingContext {
  private:
    struct sheet *sheetInfo;
    struct scale *scaleInfo;
  public:
    PlayingContext() : PlayingContext(NULL, NULL) {};
    PlayingContext(struct sheet *_sheetInfo, struct scale *_scaleInfo) : sheetInfo(_sheetInfo), scaleInfo(_scaleInfo) { };
    float get_frequency(struct note_info ni) {
      int8_t transpose;
      transpose = 0;

      if ((ni.flags & NOTE_FORCE_SHARP) && (!isSharp(scaleInfo, ni.degreeOffset)))
        ++transpose;
      if ((ni.flags & NOTE_FORCE_FLAT) && (!isFlat(scaleInfo, ni.degreeOffset)))
        --transpose;
      if ((ni.flags & NOTE_FORCE_NATURAL)) {
        if (isFlat(scaleInfo, ni.degreeOffset))
          ++transpose;
        if (isSharp(scaleInfo, ni.degreeOffset))
          --transpose;
      }
      return getFrequency( ni.degreeOffset, 5 + ni.octaveOffset, scaleInfo, transpose);
    };
    
    uint32_t getDurationMillis(struct note_info ni) {
      return getNoteLengthMillis(ni.duration, *sheetInfo);
    };
    
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
    };
    void unuse() {
      if (--nb_usage == 0) delete this;
    };
};

class Player {
   protected:
     PlayingContext *pc;
     Playable *voice;
   public:
     Player(PlayingContext *_pc, Playable *_voice) : pc(_pc), voice(_voice->useAgain()) { };
     void setVoice(Playable *_voice) {
      if (voice != NULL)
        voice->unuse();
        voice = _voice->useAgain();
      };
   virtual void play(note_info ni, unsigned long currentMillis) = 0;
   virtual void playIfReady(unsigned long currentMillis, uint8_t coordinates[]) = 0;
   virtual boolean isReady(unsigned long millis) = 0;
};

class TonePlayer : public Player {
  private :
    Tone *toneVoice;
    unsigned long nextTime;
  public:
    TonePlayer(PlayingContext *_pc, Playable *_voice, Tone *_toneVoice) : Player(_pc, _voice), toneVoice(_toneVoice), nextTime(0) { };
    void playIfReady(unsigned long currentMillis, uint8_t coordinates[]) {
      if (this->isReady(currentMillis)) {
        
      }
    };
    void play(note_info ni, unsigned long currentMillis) {
      float freq = pc->get_frequency(ni);
      uint32_t dur = pc->getDurationMillis(ni);

      if ((ni.flags & NOTE_IS_SILENCE) == 0)
        toneVoice->play(round(freq), .955*dur);
    
      // we take into accound this loop's calculus time
      nextTime = currentMillis + dur;
    };
    boolean isReady(unsigned long millis) {
      return nextTime <= millis;
    }
};

/**
 * the null playable, play nothing at all (used when a voice is not initialized)
 */
class Nothing : public Playable {
  public:
    Nothing() { };
    boolean hasMore(uint8_t *hc, uint8_t maxDepth, uint8_t depth) { return false;};
    struct note_info getOne(uint8_t *hc, uint8_t maxDepth, uint8_t depth) { return {};};
     uint8_t getMaxDepth() {return 0;};
};

 static Playable *THE_NOTHING = new Nothing();


/**
   The simplest Playable: one single playable stuff that defines a duration, callable with offset and octave
*/
class Note : public Playable {
  private:
    note_duration duration;

  public:
    Note(note_duration _duration): duration(_duration) { };
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
    RepeatHook(Playable *_p) : RepeatHook(_p, 0) { };
    RepeatHook(Playable *_p, int _nb_cycles) : p(_p->useAgain()), nb_cycles(_nb_cycles) { };

    boolean hasMore(uint8_t *hc, uint8_t maxDepth, uint8_t depth);
    struct note_info getOne(uint8_t *hc, uint8_t maxDepth, uint8_t depth);
    uint8_t getMaxDepth() ;
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
    ListHook(uint16_t _capacity);

    ListHook *add(Playable *p) {
      return add(p, 0, 0);
    };
    ListHook *add(Playable *p, int8_t degreeOffset) {
      return add(p, degreeOffset, 0);
    };
    ListHook *add(Playable *p, int8_t degreeOffset, effects flags);
    boolean hasMore(uint8_t *hc, uint8_t maxDepth, uint8_t depth);
    struct note_info getOne(uint8_t *hc, uint8_t maxDepth, uint8_t depth);
    uint8_t getMaxDepth();
};
