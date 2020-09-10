
#include <Arduino.h>
#include "hm_definitions.hpp"
struct hm_ctx {
  struct sheet *sheetInfo;
  struct scale *scaleInfo;
};




struct note_info{
  int8_t degreeOffset;
  uint8_t octaveOffset;
  note_duration duration;
  uint8_t flags;
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
    Playable *useAgain() {++nb_usage; return this;};
    void unuse() {if(--nb_usage == 0) delete this; };
};

/**
 * The simplest Playable: one single playable stuff that defines a duration, callable with offset and octave
 */
class Note : public Playable {
  private:
    note_duration duration;
    uint8_t flags;

  public:
    Note(note_duration _duration) : Note(_duration, 0) { };
    Note(note_duration _duration, uint8_t _flags): duration(_duration), flags(_flags) {};
    
    boolean hasMore(uint8_t *hc, uint8_t maxDepth, uint8_t depth){
      return hc[depth] == 0;
    };
    
    struct note_info getOne(uint8_t *hc, uint8_t maxDepth, uint8_t depth){ 
      struct note_info ret = { 0, 0, duration};
      ++(hc[depth]);
      return ret;
    };

    uint8_t getMaxDepth() {return 1;};
    
};

/**
 * A hook that repeats the inner Hook N times, or every time if N is 0
 */
class RepeatHook : public Playable {
  private:
    Playable *p;
    int nb_cycles;
    uint8_t maxDepth;
    ~RepeatHook() {
      p->unuse();
    }
  public:
    RepeatHook(Playable *_p) : RepeatHook(_p, 0) { };
    RepeatHook(Playable *_p, int _nb_cycles) : p(_p->useAgain()), nb_cycles(_nb_cycles), maxDepth(_p->getMaxDepth()) { };
    
    boolean hasMore(uint8_t *hc, uint8_t maxDepth, uint8_t depth) {
      if (nb_cycles == 0)
        return true;
      if (hc[depth] >= nb_cycles) 
        return false;
      return p->hasMore(hc, maxDepth, depth + 1);
    };

    struct note_info getOne(uint8_t *hc, uint8_t maxDepth, uint8_t depth) {
      note_info childOne = p->getOne(hc, maxDepth, depth + 1);
      if (!p->hasMore(hc, maxDepth, depth + 1)) {
        hc[depth + 1] = 0;
        ++(hc[depth]);
      }
      return childOne;
    }

    uint8_t getMaxDepth() {
      return maxDepth;
    }
};

/**
 * A hook that is a list of PlayableChild_s with no relation whatsoever.They are played one after the other
 */
class ListHook : public Playable {
  private:
    struct PlayableChild {
      int8_t degreeOffset;
      uint8_t flags;
      Playable *p;
    };
    uint8_t capacity; // max 255, todo do better with capacity, maybe use a linked list
    uint8_t number;
    uint8_t maxDepth;
    struct PlayableChild *list;
    
    ~ListHook() {
      if (list != NULL) {
        for (int i = 0; i < number; ++i)
          list[i].p->unuse();
        free(list);
        }
      };
  public:
    ListHook(uint16_t _capacity) : capacity(_capacity), number(0), maxDepth(0) {
      list = malloc(_capacity * sizeof(struct PlayableChild));
      if (list == NULL)
        Serial.println("malloc error");
    };
    ListHook *add(Playable *p) {return add(p, 0, 0); };
    ListHook *add(Playable *p, int8_t degreeOffset) {return add(p, degreeOffset, 0); };
    ListHook *add(Playable *p, int8_t degreeOffset, uint8_t flags) {
      list[number++] = {degreeOffset, flags, p->useAgain()};
      maxDepth = max(maxDepth, p->getMaxDepth() + 1);
      return this;
    }
    boolean hasMore(uint8_t *hc, uint8_t maxDepth, uint8_t depth){
      if (hc[depth] < number -1)
        return true;
      if (hc[depth] >= number)
        return false;
      list[hc[depth]].p->hasMore(hc, maxDepth, depth + 1);
    };
    struct note_info getOne(uint8_t *hc, uint8_t maxDepth, uint8_t depth) { 
      struct PlayableChild pc = list[hc[depth]];
      note_info childOne = pc.p->getOne(hc, maxDepth, depth + 1);
      if(!pc.p->hasMore(hc, maxDepth, depth + 1)) {
        hc[depth + 1] = 0;
        ++(hc[depth]);
      }
      childOne.flags |= pc.flags;
      // correction with the current structure's offset :) 
      childOne.degreeOffset += pc.degreeOffset;
      return childOne;
    }
    uint8_t getMaxDepth() {
      return maxDepth;
    }
};
