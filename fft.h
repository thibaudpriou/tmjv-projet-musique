#ifndef FFT_H
#define FFT_H

#include "common.h"
#include <complex.h>
#include <fftw3.h>

fftw_plan fft_init (complex in[FRAME_SIZE], complex spec[FRAME_SIZE]);
void fft_exit (fftw_plan);
void fft_process (fftw_plan);

#endif
