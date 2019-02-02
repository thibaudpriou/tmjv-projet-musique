#ifndef NOTE_H
#define NOTE_H

#define C4_NOTE 261.6256

#define C8_NOTE 4186.01
#define C8_SHARP_NOTE 4434.92
#define D8_NOTE 4698.63
#define D8_SHARP_NOTE 4978.03
#define E8_NOTE 5274.04
#define F8_NOTE 5587.65
#define F8_SHARP_NOTE 5919.91
#define G8_NOTE 6271.93
#define G8_SHARP_NOTE 6644.88
#define A8_NOTE 7040.00
#define A8_SHARP_NOTE 7458.62
#define B8_NOTE 7902.13

#define NB_NOTES_OCTAVE 12
#define NB_OCTAVES 8

void getNotesFrequenciesAt(int octave, double *);

void getNotesFrequenciesMatrix(double **);
void getNotesFrequenciesArray(double *);

#endif
