#ifndef HM_SCALE_HPP
#define HM_SCALE_HPP
#include <Arduino.h>
#include "hm_definitions.hpp"

static note sharp_refs[] = {NOTE_FA, NOTE_DO, NOTE_SOL, NOTE_RE, NOTE_LA, NOTE_MI, NOTE_SI};
static const uint8_t diatonic_offsets[] = {0, 2, 4, 5, 7, 9, 11};
static const uint8_t scaleSize = sizeof diatonic_offsets / sizeof diatonic_offsets[0];

class Scale {

  protected:
    char display_name[4];

  public:
    virtual float get_frequency(struct note_info ni) = 0;
    char * getName() {
      return display_name;
    }
};


class DiatonicScale : public Scale {
  private:
    uint8_t sharps;        // binary map from right to left, starting from do: 1 if sharp
    uint8_t flats;         // binary map from right to left, starting from do: 1 if flat
    note note_base;        // the NOTE_something telling which note is the Tonic
    uint8_t octave;        // 0 based octave offset
    int8_t getAccidentCorrection(struct note_info ni);
    bool isSharp (byte degre);
    bool isFlat (byte degre);
    bool isNatural (byte degre);
    uint16_t getMidiNote(const int8_t degree, const uint8_t octave);
  public:
    /* the number of accident, # if accidents are sharps, b if they are flats, then M for major, m for minor
      doM -> "0#M" or "0bM" are valid
      lam -> "0#m" or "0bm" are valid
    */
    static bool checkScaleInitParam(const char *c);
    DiatonicScale(const char *c);
    float get_frequency(struct note_info ni);
};


#endif // HM_SCALE_HPP
