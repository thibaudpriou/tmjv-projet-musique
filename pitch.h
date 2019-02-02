#ifndef PITCH_H
#define PITCH_H

#define MODE_MAJOR 0
#define MODE_MINOR 1

double* get_major_profil();
double* get_minor_profil();

double computeCorrelation(int*, double*, int, int);
double detect_pitch(int*, double*, int *);
void detect_pitch_mode(int*, int *, int *);

#endif
