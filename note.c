#include "note.h"
#include "math.h"
#include <stdlib.h>
#include <string.h>

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

void
note2string(int note, char* string)
{
  switch (note) {
    case 0:
      strcpy(string, "C");
      break;
    case 1:
      strcpy(string, "C#");
      break;
    case 2:
      strcpy(string, "D");
      break;
    case 3:
      strcpy(string, "D#");
      break;
    case 4:
      strcpy(string, "E");
      break;
    case 5:
      strcpy(string, "F");
      break;
    case 6:
      strcpy(string, "F#");
      break;
    case 7:
      strcpy(string, "G");
      break;
    case 8:
      strcpy(string, "G#");
      break;
    case 9:
      strcpy(string, "A");
      break;
    case 10:
      strcpy(string, "A#");
      break;
    case 11:
      strcpy(string, "B");
      break;
    default:
      string = NULL;
      break;
  }
}
