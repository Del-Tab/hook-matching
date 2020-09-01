
#include <Arduino.h>
#include "hm_music.hpp"
struct hm_ctx {
  struct sheet *sheetInfo;
  struct scale *scaleInfo;
};

#define NOTE_IS_SILENCE = 1;
#define NOTE_STICKS_TO_NEXT = 2;
struct tone_info {
  float frequence;
  uint32_t duration_ms;
  uint8_t flags;
};



class Playable {
  private:
    int nb_usage;
  public: 
    virtual void * getIteratorInitCtx() = 0;
    virtual boolean hasNext(void * ctx) = 0;
    virtual struct tone_info getNext(void * iterCtx, struct hm_ctx *hm_context, int8_t degreeOffset, uint8_t octave) = 0;
    
    // linked to the memory usage:
    Playable(): nb_usage(1) {}; // we suppose it's used once when allocated . call unuse to free
    Playable *useAgain() {++nb_usage; return this;};
    void unuse() {if(--nb_usage == 0) delete this; };
};
/*class PlayableIterator {
  private:
    void* ctx;
    Playable *p;
  public:
    PlayableIterator(Playable *p): ctx(p->getIteratorInitCtx()), p(p) { };
    struct tone_info getNext() {return p->getNext(ctx);};
    boolean hasNext() {return p->hasNext(ctx);};
};*/

/**
 * The simplest Playable: one single playable stuff that defines a duration, callable with offset and octave
 */
class Note : public Playable {
  private:
    note_duration duration;
    uint8_t flags;
  // degreeOffset ~ partie.hook + degreeOffset of parent
  int play(void *play_ctx, struct hm_ctx *hm_context, int8_t degreeOffset, uint8_t octave) {
    float freq = getFrequency(hm_context->scaleInfo->note_base + degreeOffset, octave, hm_context->scaleInfo);
    uint32_t dur = getNoteLengthMillis(duration, *hm_context->sheetInfo);  

  }
  public:
    Note(note_duration _duration, uint8_t _flags): duration(_duration), flags(_flags) {};
    void * getIteratorInitCtx() {
      bool * ret = malloc(sizeof(boolean));
      *ret = true;
      return ret;
    };
    boolean hasNext(void * iterCtx) {return *((bool*)iterCtx);};
    struct tone_info getNext(void *iterCtx, struct hm_ctx *hm_context, int8_t degreeOffset, uint8_t octave) {
      float freq = getFrequency(hm_context->scaleInfo->note_base + degreeOffset, octave, hm_context->scaleInfo);
      uint32_t dur = getNoteLengthMillis(duration, *hm_context->sheetInfo);  
      *((boolean *)iterCtx) = false;
      struct tone_info ret = {
        freq, dur, flags
        };
      return ret;
    };
};

/**
 * A hook that is a list of Playables
 */
class ListHook : Playable {
  
};
