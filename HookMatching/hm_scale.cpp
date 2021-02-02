#include "hm_scale.hpp"
#include "hm_definitions.hpp"
#include "hm_maths.hpp"

static note sharp_refs[] = {NOTE_FA, NOTE_DO, NOTE_SOL, NOTE_RE, NOTE_LA, NOTE_MI, NOTE_SI};
static const uint8_t diatonic_offsets[] = {0, 2, 4, 5, 7, 9, 11};
static const uint8_t scaleSize = sizeof diatonic_offsets / sizeof diatonic_offsets[0];

/*BEGIN DiatonicScale implementation*/
DiatonicScale::DiatonicScale(const char *desc) {
  octave = 5;
  sharps = flats = 0;
  uint8_t num = desc[0] - '0';
  if (num == 0) {
    note_base = NOTE_DO;
  } else {
    if (desc[1] == '#') {
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
  if (desc[2] == 'm') {
    isMinor = true;
    note_base = hm_mod(note_base - 2 , 7);
  } else if (desc[2] != 'M') {
    note chosen_note = hm_mod(desc[2] - 'C', 7);
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

int8_t DiatonicScale::getAccidentCorrection(struct note_info ni) {
  int8_t transpose = 0;
  if (ni.flags & NOTE_FORCE_SHARP) {
    if (!isSharp(ni.degreeOffset))
      ++transpose;
    if (isFlat(ni.degreeOffset))
      ++transpose;
  }

  if (ni.flags & NOTE_FORCE_FLAT) {
    if (!isFlat(ni.degreeOffset))
      --transpose;
    if (isSharp(ni.degreeOffset))
      --transpose;
  }

  if ((ni.flags & NOTE_FORCE_NATURAL)) {
    if (isFlat(ni.degreeOffset))
      ++transpose;
    if (isSharp(ni.degreeOffset))
      --transpose;
  }
  return transpose;
}

bool DiatonicScale::isSharp (byte degre) {
  return (1 << hm_mod(degre + note_base, 7)) & sharps;
}

bool DiatonicScale::isFlat (byte degre) {
  return (1 << hm_mod(degre + note_base, 7)) & flats;
}

bool DiatonicScale::isNatural (byte degre) {
  return !(isSharp(degre) ^ isFlat(degre));
}

uint16_t DiatonicScale::getMidiNote(const int8_t degree, const uint8_t octave) {
  uint16_t ret = (octave  + hm_div((degree + note_base), scaleSize)) * 12
                 + diatonic_offsets[hm_mod((hm_mod(degree, scaleSize) + note_base), scaleSize)];
  if (isSharp(degree))
    ++ret;
  if (isFlat(degree))
    --ret;
  return ret;
}

float DiatonicScale::get_frequency(struct note_info ni) {

  int8_t transpose = getAccidentCorrection(ni);
  // Getting base do (C) from current octave ...
  int16_t offsetFromLa440 = getMidiNote(ni.degreeOffset, octave + ni.octaveOffset) + transpose;
  offsetFromLa440 -= 5 * 12 + diatonic_offsets[NOTE_LA]; //LA440 is on octave 5 (0 based)
  // getting the right frequency from the ref and offset
  float freq = LA4_REF * pow(2.0, offsetFromLa440 / 12.0);

  return freq;
}
bool DiatonicScale::checkScaleInitParam(const char *c) {

  if (strlen(c) != 3)
    return false;
  if (c[0] < '0' && c[0] > '7')
    return false;
  if (c[1] != '#' && c[1] != 'b')
    return false;
  if (c[2] != 'm' && c[1] != 'M' && c[1] < 'A' && c[1] > 'C') // either 1st note or M/m for major/minor
    return false;
  return true;
}

/*END DiatonicScale implementation*/
