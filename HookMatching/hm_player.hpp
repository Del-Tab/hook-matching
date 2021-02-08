/*
   please keep this reference in this file when using this code anywhere
   https://github.com/DelTa-B/hook-matching/
   I would be glad if you give this link when you take part of this file :)
*/
#ifndef HM_PLAYER_HPP
#define HM_PLAYER_HPP


#include "hm_maths.hpp"
#include "hm_scale.hpp"
#include <Tone.h>

#ifndef MAX_DEPTH
# define MAX_DEPTH 16
#endif
#ifndef HM_PULSE_STEP
# define HM_PULSE_STEP .075
#endif

class playing_context {
  private:
    struct sheet_dep *sheetInfo;
    scale *scaleInfo;
  public:
    playing_context(struct sheet_dep *a_sheetInfo, scale *a_scale = NULL);
    struct sheet_dep *getSheetInfo();
    float get_frequency(const struct note_info *ni);
    uint32_t getDurationMillis(const struct note_info *ni);
};

class playable {
  private:
    unsigned int nb_usage;
  public:
    virtual boolean hasMore(uint8_t *hc, uint8_t depth) = 0;
    virtual struct note_info getOne(uint8_t *hc, uint8_t depth)  = 0;
    virtual uint8_t getMaxDepth() = 0;

    // linked to the memory usage:
    playable();
    playable *useAgain();
    void unuse(); // call this any time you don't use the pointer anymore, it'll be freed once the counter reaches 0
};

class player {
  // TODO ref3 : make private what shall be + hide implementation in pimpl like it's done in the diatonic_scale class
  protected:
    playing_context *pc;
    playable *voice;
    // TODO ref1 : better coordinates
    uint8_t coordinates[MAX_DEPTH];
    unsigned long nextTime;
    boolean isReady(unsigned long millis);
  public:
    player(playing_context *a_pc, playable *a_voice);
    boolean hasFinished();
    void setVoice(playable *n_voice);
    virtual void playIfReady(unsigned long currentMillis) = 0;
};


/*

*/
class tone_player : public player {
  private :
    Tone *toneVoice;
  public:
    tone_player(playing_context *a_pc, uint8_t a_tonePin, playable *a_voice);
    void playIfReady(unsigned long currentMillis);
};




/**
   the null playable, play nothing at all (used when a voice is not initialized and you want to call methods without crashing)
*/
class nothing : public playable {
  public:
    boolean hasMore(uint8_t *hc, uint8_t depth);
    struct note_info getOne(uint8_t *hc, uint8_t depth);
    uint8_t getMaxDepth();
};

static playable *THE_NOTHING = new nothing();


/**
   The simplest playable: one single playable stuff that defines a duration, callable with offset and octave
*/
class note : public playable {
  private:
    note_duration duration;

  public:
    note(note_duration a_duration): duration(a_duration) { }
    boolean hasMore(uint8_t *hc, uint8_t depth);
    struct note_info getOne(uint8_t *hc, uint8_t depth);
    uint8_t getMaxDepth();
};



/**
   A hook that repeats the inner Hook N times, or every time if N is 0
*/
class repeat_hook : public playable {
  private:
    playable *p;
    uint8_t nb_cycles;
    ~repeat_hook();
  public:
    repeat_hook(playable *a_p, uint8_t a_nb_cycles = 0);
    boolean hasMore(uint8_t *hc, uint8_t depth);
    struct note_info getOne(uint8_t *hc, uint8_t depth);
    uint8_t getMaxDepth();
};

/**
   A hook that is a list of PlayableChild_s with no relation whatsoever.They are played one after the other
*/
class list_hook : public playable {
  private:
    struct PlayableChild {
      hm_offset degreeOffset;
      effects flags;
      playable *p;
    };
    uint8_t capacity; // max 255, todo do better with capacity, maybe use a linked list
    uint8_t number;
    uint8_t maxDepth;
    struct PlayableChild *list;
    ~list_hook();
  public:
    list_hook(uint8_t a_capacity);

    list_hook *add(playable *p, int8_t degreeOffset = 0, effects flags = 0);
    boolean hasMore(uint8_t *hc, uint8_t depth);
    struct note_info getOne(uint8_t *hc, uint8_t depth);
    uint8_t getMaxDepth();
};


class led_metronome : public player {
  private:
    unsigned int pulseLed;
    int brightness;
  public:
    led_metronome(playing_context *_pc, uint8_t _pulseLed);
    void playIfReady(unsigned long currentMillis);
};
#endif // HM_PLAYER_HPP
