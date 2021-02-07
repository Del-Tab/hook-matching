#ifndef HM_SCALE_HPP
#define HM_SCALE_HPP
#include "hm_definitions.hpp"


class scale {
  protected:
    char display_name[4];
  public:
    virtual float get_frequency(struct note_info ni) = 0;
    char * getName() {
      return display_name;
    }
};
// TODO ref2 put it in a sheet class
struct sheet_dep {
  scale *default_scale;
  uint16_t bpm;           // most of the time ranges from 96 to 480, but you're free, just don't chose 0, and consider we are millisecond precise only
  note_duration bpm_unit; // 24 when bpm is expressed on the quarter note, 48 on the half note, etc
  uint8_t top;            // number of time unit per case
  note_duration bottom;   // if it's 4, quarter notes are one time unit, if it's 2, half notes are 1 time unit, if it's 8 eights are 1 time unit etc
};
uint32_t getNoteLengthMillis(note_duration nd, const struct sheet_dep & p);

class diatonic_scale : public scale {
  private:
    class impl;        // private stuff go there
    impl* pimpl_;      // opaque pointer to forward-declared class
  public:
    /*  -the number of accident on 1 char,
        -then # if accidents are sharps, b if they are flats,
        -then M for major, m for minor or the base note
      minorE -> "1#m" or "1#E"
      majorF -> "1bM" of "1bF"
      majorC -> "0#M" or "0bM" (or "0#C" or "0bC")
      minorA -> "0#m" or "0bm" (or "0#A" or "0bA")
    */
    static bool checkScaleInitParam(const char *desc);
    diatonic_scale(const char *desc);
    float get_frequency(struct note_info ni);
};


#endif // HM_SCALE_HPP
