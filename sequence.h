#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "samples.h"
#include "common.h"
#include "chromas.h"

#define CHROMA_SIZE 12
#define MFCC_SIZE 32

typedef struct sequence {
  int chroma_size;
  double *chromas;
  int fft_size;
  complex *fft;
} sequence;

sequence init_sequence(int chroma_size, int fft_size);
sequence* fill_sequence(char* infilename, int *nb_frames, int *samplerate, int fft_size, int hop_size, int chroma_size);
void free_sequence(sequence *seqs, int nb_frames);

float cosine_distance(sequence s1, sequence s2);

float** calculate_similarity_matrix(char *imageName, sequence *sequence1, int nb_frames1, sequence *sequence2, int nb_frames2);
float ** calculate_autosimilarity_matrix(char *imageName, sequence *seq, int nb_frames);

double calculate_tempo(sequence *seq, int nb_frames, int samplerate, int fft_size, int hop_size);

void calculate_pitch(sequence *seqs, int nb_frames, int *pitch, int *mode);


#endif
