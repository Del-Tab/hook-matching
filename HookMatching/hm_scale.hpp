#ifndef HM_SCALE_HPP
#define HM_SCALE_HPP
#include <Arduino.h>
#include "hm_definitions.hpp"


class Scale {
  protected:
    char display_name[4];
  public:
    virtual float get_frequency(struct note_info ni) = 0;
    char * getName() {
      return display_name;
    }
};


class diatonic_scale : public Scale {
  private:
    // TODO ref3 hide also private methods inside a class instead of a struct
    class impl;        // things to be hidden go here
    impl* pimpl_;      // opaque pointer to forward-declared class
  public:
    /*  -the number of accident on 1 char,
        -then # if accidents are sharps, b if they are flats,
        -then M for major, m for minor or the base note
      minorE -> "1#m" or "1#E"
      majorC -> "0#M" or "0bM" (or "0#C" or "0bC") are valid
      minorA -> "0#m" or "0bm" (or "0#A" or "0bA") are valid
    */
    static bool checkScaleInitParam(const char *c);
    diatonic_scale(const char *desc);
    float get_frequency(struct note_info ni);
};


#endif // HM_SCALE_HPP
