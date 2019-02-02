#ifndef CHROMA_H
#define CHROMA_H

#include "fft.h"

int freq2chro(double freq, int chroma_size);
void computeChromas(complex *, size_t, double, double*);

#endif
