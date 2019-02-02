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
  printf("\n Usage : %s <input file> <output file>(opt)\n", progname);
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
  sequence *seqs = fill_sequence(infilename, &nb_frames_seq, &samplerate, CHROMA_SIZE);

  if (seqs == NULL){
    printf("erreur calcul sequence \n");
    return 1;
  }

  /* TEMPO ESTIMATION */
  double tempo = calculate_tempo(seqs, nb_frames_seq, samplerate);
  printf("Tempo en BPM %f\n", tempo);

	/* TEMPO ESTIMATION */
  calculate_autosimilarity_matrix(outputfilename, seqs, nb_frames_seq);

	/* TEMPO ESTIMATION */
	int mode;
	int pitch;
	calculate_pitch(seqs, nb_frames_seq, &pitch, &mode);
	printf("Mode %s\n", (mode == MODE_MAJOR) ? "Majeur" : "Mineur");
	printf("Pitch : %d\n", pitch);


  return 0;
} /* main */
