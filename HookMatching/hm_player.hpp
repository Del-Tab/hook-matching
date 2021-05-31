/*
   please keep this reference in this file when using this code anywhere
   https://github.com/DelTa-B/hook-matching/
   I would be glad if you give this link when you take part of this file :)
*/
#ifndef HM_PLAYER_HPP
#define HM_PLAYER_HPP

#include "hm_definitions.hpp"


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

const dac_data hm_SIN[256] PROGMEM = {0x0FFF, 0x0FFE, 0x0FFC, 0x0FF9, 0x0FF5, 0x0FEF, 0x0FE8, 0x0FE0, 0x0FD7, 0x0FCD, 0x0FC1, 0x0FB4, 0x0FA6, 0x0F97, 0x0F87, 0x0F75, 0x0F63, 0x0F4F, 0x0F3A, 0x0F24, 0x0F0D, 0x0EF5, 0x0EDB, 0x0EC1, 0x0EA5, 0x0E89, 0x0E6C, 0x0E4D, 0x0E2E, 0x0E0D, 0x0DEC, 0x0DCA, 0x0DA7, 0x0D83, 0x0D5E, 0x0D38, 0x0D12, 0x0CEB, 0x0CC3, 0x0C9A, 0x0C71, 0x0C46, 0x0C1C, 0x0BF0, 0x0BC4, 0x0B98, 0x0B6A, 0x0B3D, 0x0B0F, 0x0AE0, 0x0AB1, 0x0A81, 0x0A51, 0x0A21, 0x09F1, 0x09C0, 0x098E, 0x095D, 0x092B, 0x08FA, 0x08C8, 0x0896, 0x0863, 0x0831, 0x07FF, 0x07CD, 0x079B, 0x0768, 0x0736, 0x0704, 0x06D3, 0x06A1, 0x0670, 0x063E, 0x060D, 0x05DD, 0x05AD, 0x057D, 0x054D, 0x051E, 0x04EF, 0x04C1, 0x0494, 0x0466, 0x043A, 0x040E, 0x03E2, 0x03B8, 0x038D, 0x0364, 0x033B, 0x0313, 0x02EC, 0x02C6, 0x02A0, 0x027B, 0x0257, 0x0234, 0x0212, 0x01F1, 0x01D0, 0x01B1, 0x0192, 0x0175, 0x0159, 0x013D, 0x0123, 0x0109, 0x00F1, 0x00DA, 0x00C4, 0x00AF, 0x009B, 0x0089, 0x0077, 0x0067, 0x0058, 0x004A, 0x003D, 0x0031, 0x0027, 0x001E, 0x0016, 0x000F, 0x0009, 0x0005, 0x0002, 0x0000, 0x0000, 0x0000, 0x0002, 0x0005, 0x0009, 0x000F, 0x0016, 0x001E, 0x0027, 0x0031, 0x003D, 0x004A, 0x0058, 0x0067, 0x0077, 0x0089, 0x009B, 0x00AF, 0x00C4, 0x00DA, 0x00F1, 0x0109, 0x0123, 0x013D, 0x0159, 0x0175, 0x0192, 0x01B1, 0x01D0, 0x01F1, 0x0212, 0x0234, 0x0257, 0x027B, 0x02A0, 0x02C6, 0x02EC, 0x0313, 0x033B, 0x0364, 0x038D, 0x03B8, 0x03E2, 0x040E, 0x043A, 0x0466, 0x0494, 0x04C1, 0x04EF, 0x051E, 0x054D, 0x057D, 0x05AD, 0x05DD, 0x060D, 0x063E, 0x0670, 0x06A1, 0x06D3, 0x0704, 0x0736, 0x0768, 0x079B, 0x07CD, 0x07FF, 0x0831, 0x0863, 0x0896, 0x08C8, 0x08FA, 0x092B, 0x095D, 0x098E, 0x09C0, 0x09F1, 0x0A21, 0x0A51, 0x0A81, 0x0AB1, 0x0AE0, 0x0B0F, 0x0B3D, 0x0B6A, 0x0B98, 0x0BC4, 0x0BF0, 0x0C1C, 0x0C46, 0x0C71, 0x0C9A, 0x0CC3, 0x0CEB, 0x0D12, 0x0D38, 0x0D5E, 0x0D83, 0x0DA7, 0x0DCA, 0x0DEC, 0x0E0D, 0x0E2E, 0x0E4D, 0x0E6C, 0x0E89, 0x0EA5, 0x0EC1, 0x0EDB, 0x0EF5, 0x0F0D, 0x0F24, 0x0F3A, 0x0F4F, 0x0F63, 0x0F75, 0x0F87, 0x0F97, 0x0FA6, 0x0FB4, 0x0FC1, 0x0FCD, 0x0FD7, 0x0FE0, 0x0FE8, 0x0FEF, 0x0FF5, 0x0FF9, 0x0FFC, 0x0FFE};
//todo having also an envelop
class waveform {
  public:
    const dac_data ** wave;
    uint16_t len;
    waveform(const dac_data ** a_wave, uint16_t a_len): wave(a_wave),len(a_len) { };
};

static waveform* THE_SIN = new waveform((const dac_data **)&hm_SIN, sizeof hm_SIN);

class dac_player : public player {
  private:
    class impl;        // private stuff go there
    impl* pimpl_;      // opaque pointer to forward-declared class
  public:
    dac_player(playing_context *a_pc, waveform *a_waveform, playable *a_voice);
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
