#ifndef SAMPLES_H
#define SAMPLES_H

#include "lib/sndfile.h"
#include "common.h"

void fill_buffer(double *, double *);
int read_n_samples (SNDFILE *, double *, int, int);
int write_n_samples (SNDFILE *, double *, int, int);
int read_samples (SNDFILE *, double *, int);
int write_samples (SNDFILE *, double *, int);

#endif
