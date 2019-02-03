#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lib/sndfile.h"

#include <math.h>
#include "lib/gnuplot_i.h"

#include "rythme.h"
#include "samples.h"
#include "fft.h"
#include "note.h"
#include "pitch.h"
#include "chromas.h"
#include "sequence.h"

void
print_usage(char *progname)
{
  printf("\n Usage : %s <input file> <output file>\n", progname);
  puts("\n");
}

int
main(int argc, char * argv [])
{
  char 		*progname, *infilename, *outputfilename;

  progname = strrchr(argv[0], '/');
  progname = progname ? progname + 1 : argv[0];

	if(argc != 3) {
    print_usage(progname);
    return 1;
  }

	infilename = argv[1];
  outputfilename = argv[2];

	int nb_frames_seq = 0;
  int samplerate = 0;
  sequence *seqs;
  seqs = fill_sequence(infilename, &nb_frames_seq, &samplerate, FRAME_SIZE, HOP_SIZE, CHROMA_SIZE);

  if (seqs == NULL){
    printf("erreur calcul sequence \n");
    return 1;
  }

  /* TEMPO ESTIMATION */
  double tempo = calculate_tempo(seqs, nb_frames_seq, samplerate, FRAME_SIZE, HOP_SIZE);
  printf("Tempo en BPM : %f\n", tempo);

	/* AUTOSIMILARITY MATRIX */
  calculate_autosimilarity_matrix(outputfilename, seqs, nb_frames_seq);
  printf("Image d'autosimilarité sauvegardée : %s\n", outputfilename);

	free_sequence(seqs, nb_frames_seq);

	/* PITCH ESTIMATION */
  nb_frames_seq = 0;
  samplerate = 0;
	sequence *seqs2 = fill_sequence(infilename, &nb_frames_seq, &samplerate, PITCH_FRAME_SIZE, PITCH_HOP_SIZE, CHROMA_SIZE);

	if (seqs2 == NULL){
		printf("erreur calcul sequence \n");
		return 1;
	}

	int mode;
	int pitch;
	calculate_pitch(seqs2, nb_frames_seq, &pitch, &mode);
	printf("Mode : %s\n", (mode == MODE_MAJOR) ? "Majeur" : "Mineur");
	printf("Pitch : %d\n", pitch);

	free_sequence(seqs2, nb_frames_seq);

  return 0;
} /* main */
