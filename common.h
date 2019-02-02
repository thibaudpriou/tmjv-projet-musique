#ifndef COMMON_H
#define COMMON_H

// #define HOP_SIZE 256
// #define	FRAME_SIZE 512


// This FRAME_SIZE allows us to correctly identify notes from a FFT
// #define HOP_SIZE 2018
// #define	FRAME_SIZE 4096
#define HOP_SIZE 16384
#define	FRAME_SIZE 32768

int my_modulo(int x, int y);

#endif
