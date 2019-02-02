#include "chromas.h"
#include "fft.h"
#include "common.h"
#include "note.h"

#include <math.h>

int
freq2chro(double freq, int chroma_size)
{
  return (int) round( chroma_size * log2( freq / C4_NOTE ) );
}

int
freq2chro_idx(double freq, int chroma_size)
{
  return my_modulo(freq2chro(freq, chroma_size) , chroma_size);
}

void
computeChromas(complex *fft, size_t fft_size, double sample_rate, double chromas[NB_NOTES_OCTAVE])
{
  int bin_chroma;
  double freq, amplitude;
  for (size_t i = 0; i < fft_size/2; i++)
	{
    freq = fft_index2freq(i, fft_size, sample_rate);
	  bin_chroma = freq2chro_idx(freq, NB_NOTES_OCTAVE);
    amplitude = cabs(fft[i]);
    chromas[bin_chroma] += amplitude;
	}
}
