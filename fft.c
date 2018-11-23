#include "fft.h"

fftw_plan
fft_init (complex in[FRAME_SIZE], complex spec[FRAME_SIZE])
{
  return fftw_plan_dft_1d (FRAME_SIZE, in, spec, FFTW_FORWARD, FFTW_ESTIMATE);
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
