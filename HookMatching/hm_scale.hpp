#ifndef HM_SCALE_HPP
#define HM_SCALE_HPP
#include <Arduino.h>
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


class diatonic_scale : public scale {
  private:
    // TODO ref3 hide also private methods inside a class instead of a struct
    class impl;        // things to be hidden go here
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
