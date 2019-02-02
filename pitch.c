#include "pitch.h"
#include "common.h"
#include "note.h"

double*
get_major_profil()
{
  static double krumhansl_major[NB_NOTES_OCTAVE] = {6.35,2.23,3.48,2.33,4.38,4.09,2.52,5.19,2.39,3.66,2.29,2.88};
  return krumhansl_major;
}

double*
get_minor_profil()
{
  static double krumhansl_minor[NB_NOTES_OCTAVE] = {6.33,2.68,3.52,5.38,2.6,3.53,2.54,4.75,3.98,2.69,3.34,3.17};
  return krumhansl_minor;
}

double
computeCorrelation(int* hist1, double* hist2, int size, int offset)
{
  double correlation = 0.0;
  for (int p = 0; p < size; p++) {
      correlation += hist1[p] * hist2[(offset + p) % size];
  }
  return correlation;
}

double
detect_pitch(int histogram[NB_NOTES_OCTAVE], double profil[NB_NOTES_OCTAVE], int *pitch)
{
  double max_correlation = 0.0;
  double correlation;

  for (int i = 0; i < NB_NOTES_OCTAVE; i++) {

    correlation = computeCorrelation(histogram, profil, NB_NOTES_OCTAVE, i);

    if (correlation > max_correlation) {
        max_correlation = correlation;
        *pitch = i;
    }
  }
  return max_correlation;
}

void
detect_pitch_mode(int histogram[NB_NOTES_OCTAVE], int *pitch, int *mode)
{
  int major_pitch = 0, minor_pitch = 0;

  double* minor_profil = get_minor_profil();
  double* major_profil = get_minor_profil();

  double major_corr = detect_pitch(histogram, major_profil, &major_pitch);
  double minor_corr = detect_pitch(histogram, minor_profil, &minor_pitch);

  if (major_corr > minor_corr) {
    *pitch = major_pitch;
    *mode = MODE_MAJOR;
  } else {
    *pitch = minor_pitch;
    *mode = MODE_MINOR;
  }
}
