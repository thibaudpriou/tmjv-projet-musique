#include "fft.h"
#include "math.h"

fftw_plan
fft_init (complex *in, complex *spec, int fft_size)
{
  return fftw_plan_dft_1d (fft_size, in, spec, FFTW_FORWARD, FFTW_ESTIMATE);
}

void
fft_exit (fftw_plan plan)
{
    fftw_destroy_plan(plan);
    fftw_cleanup();
}

void
fft_process (fftw_plan plan)
{
  fftw_execute (plan);
}

double
fft_index2freq(int index, size_t fft_size, double sampling_rate)
{
	return index * sampling_rate / fft_size;
}

int
fft_freq2index(double freq, size_t fft_size, double sampling_rate)
{
	return ceil(freq * fft_size / sampling_rate);
}

void
fft_freqs2indexes(double *freq, size_t freq_size, size_t fft_size, double sampling_rate, int *indexes)
{
  for (size_t i = 0; i < freq_size; i++) {
    indexes[i] = fft_freq2index(freq[i], fft_size, sampling_rate);
  }
}
