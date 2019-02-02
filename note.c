#include "note.h"
#include "math.h"
#include <stdlib.h>

double *
getNotesFrequencies8()
{
  static double notes[NB_NOTES_OCTAVE] = {
    C8_NOTE,
    C8_SHARP_NOTE,
    D8_NOTE,
    D8_SHARP_NOTE,
    E8_NOTE,
    F8_NOTE,
    F8_SHARP_NOTE,
    G8_NOTE,
    G8_SHARP_NOTE,
    A8_NOTE,
    A8_SHARP_NOTE,
    B8_NOTE,
  };

  return notes;
}

void
getNotesFrequenciesAt(int octave, double *freqs)
{
  if (octave < 0 || octave > 8) {
    freqs = NULL;
    return;
  }

  double *notes = getNotesFrequencies8();
  octave -= 8;

  for (int i = 0; i < NB_NOTES_OCTAVE; i++) {
    freqs[i] = notes[i] / pow(2, octave);
  }
}

void
getNotesFrequenciesMatrix(double ** freqs)
{
  for (int i = 0; i < NB_OCTAVES; i++) {
    getNotesFrequenciesAt(i, freqs[i]);
  }
}

void
getNotesFrequenciesArray(double * freqs)
{
  double octave[NB_NOTES_OCTAVE];
  for (int i = 0; i < NB_OCTAVES; i++) {
    getNotesFrequenciesAt(i, octave);

    for (int j = 0; j < NB_NOTES_OCTAVE; j++) {
      freqs[j + i*NB_NOTES_OCTAVE] = octave[j];
    }
  }
}
