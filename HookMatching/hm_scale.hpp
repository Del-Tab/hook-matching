#ifndef HM_SCALE_HPP
#define HM_SCALE_HPP

#include "hm_definitions.hpp"

static note sharp_refs[] = {NOTE_FA, NOTE_DO, NOTE_SOL, NOTE_RE, NOTE_LA, NOTE_MI, NOTE_SI};

class Scale {
  
  protected:
    char display_name[4];

  public:
    char * getName() {
      return display_name;
    }
};


class DiatonicScale : public Scale {
  private:
    uint8_t sharps;        // binary map from right to left, starting from do: 1 if sharp
    uint8_t flats;         // binary map from right to left, starting from do: 1 if flat
    note note_base;        // the NOTE_something telling which note is the Tonic
    
  public:

    /* the number of accident, # if accidents are sharps, b if they are flats, then M for major, m for minor
      doM -> "0#M" or "0bM" are valid

    */
    static bool checkScaleInitParam(char *c) {

      if (strlen(c) != 3)
        return false;
      if (c[0] < '0' && c[0] > '7')
        return false;
      if (c[1] != '#' && c[1] != 'b')
        return false;
      if (c[2] != 'm' && c[1] != 'M' && c[1] < 'A' && c[1] > 'C') // either 1st note or major/minor
        return false;
      return true;
    }
    DiatonicScale(char *c) {
      sharps = flats = 0;
      uint8_t num = c[0] - '0';
      if (num == 0) {
        note_base = NOTE_DO;
      } else {
        if (c[1] == '#') {
          for (uint8_t i = 0; i < num; ++i) {
            sharps |= 1 << sharp_refs[i];
          }
          note_base = sharp_refs[num - 1] + 1 % 7;
        } else {
          for (uint8_t i = 0; i < num; ++i) {
            flats |= 1 << sharp_refs[(sizeof sharp_refs) - 1 - i];
          }
          note_base = sharp_refs[hm_mod((1 - num), sizeof sharp_refs)];
        }
      }
      bool isMinor = false;
      if (c[2] == 'm') {
        isMinor = true;
        note_base = hm_mod(note_base - 2 , 7);
      } else if (c[2] != 'M') {
        note chosen_note = hm_mod(c[2] - 'C', 7);
        if (chosen_note == hm_mod(note_base - 2, 7))
          isMinor = true;
        note_base = chosen_note;
      }
      display_name[0] = 'C' + note_base;
      if (display_name[0] > 'G')
        display_name[0] -= 7;
      int writeOffset = 1;
      if ((1 << hm_mod(note_base, 7)) & sharps)
        display_name[writeOffset++] = '#';
      else if ((1 << hm_mod(note_base, 7)) & flats)
        display_name[writeOffset++] = 'b';
      if (isMinor)
        display_name[writeOffset++] = 'm';
      else
        display_name[writeOffset++] = 'M';
      display_name[writeOffset++] = '\0';
    }
};
#endif // HM_SCALE_HPP
