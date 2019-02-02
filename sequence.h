#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <stdio.h>
#include <stdlib.h>

//#define SIZE 12 //32 if MFCC, 12 if CHROMA

typedef struct sequence {
  int size;
  double *data;
} sequence;

sequence* create_sequence(int size);

float cosine_distance(sequence s1, sequence s2);

float** calculate_similarity_matrix(char *imageName, sequence *sequence1, int nb_frames1, sequence *sequence2, int nb_frames2);
float ** calculate_autosimilarity_matrix(char *imageName, sequence *seq, int nb_frames);

#endif
