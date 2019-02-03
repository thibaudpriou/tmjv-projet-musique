#include "sequence.h"
#include "rythme.h"
#include "pitch.h"

sequence
init_sequence(int chroma_size, int fft_size)
{
  sequence *s = malloc(sizeof(struct sequence));
  s->chroma_size = chroma_size;
  s->chromas = malloc(chroma_size * sizeof(double));
  s->fft_size = fft_size;
  s->fft = malloc(fft_size * sizeof(complex));
  return *s;
}

sequence*
fill_sequence(char* infilename, int *nb_frames, int *samplerate, int fft_size, int hop_size, int chroma_size)
{
  SNDFILE	 	*infile = NULL ;
  SF_INFO	 	sfinfo ;

  if ((infile = sf_open (infilename, SFM_READ, &sfinfo)) == NULL){
    printf ("Not able to open input file %s.\n", infilename) ;
    puts (sf_strerror (NULL)) ;
    return NULL ;
  }

  /* Read WAV and Compute Chromas */
  *nb_frames = 0;
  double new_buffer[hop_size];
  double buffer[fft_size];

  int i;
  for (i=0; i<( fft_size/hop_size -1); i++){
    if (read_samples (infile, new_buffer, sfinfo.channels, hop_size) == 1){
    	fill_buffer(buffer, new_buffer, fft_size, hop_size);
    } else {
  	  printf("not enough samples !!\n");
  	  return NULL;
  	}
  }

  *samplerate = (int)sfinfo.samplerate;

  // printf("Taille : %d échantillons \n", (int)sfinfo.frames);
  int seq_size = (int)ceil((double) sfinfo.frames / hop_size);
  // printf("Calcul de FFT sur une taille de %d échantillons\n", fft_size);
  // printf("Nombres séquences : %d \n", seq_size);

  sequence* seqs = malloc(sizeof(struct sequence) * seq_size);

  complex samples[fft_size];
  complex spec[fft_size];

  double *chromas = malloc(chroma_size * sizeof(double));

  /* FFT init */
  fftw_plan fft = fft_init(samples, spec, fft_size);

  while (read_samples (infile, new_buffer, sfinfo.channels, hop_size) == 1){
    seqs[(*nb_frames)] = init_sequence(chroma_size, fft_size);

    /* hop size */
    fill_buffer(buffer, new_buffer, fft_size, hop_size);

    // fft input
    for (i = 0; i < fft_size; i++){
    	samples[i] = buffer[i];
    }

    // compute fft
    fft_process(fft);

    // save fft
    for (i = 0; i < fft_size; i++){
      seqs[(*nb_frames)].fft[i] = spec[i];
  	}

    // compute chromas
    computeChromas(seqs[(*nb_frames)].fft, fft_size, *samplerate, chromas);
    for (i = 0; i < chroma_size; i++) {
  	  seqs[(*nb_frames)].chromas[i] = chromas[i];
  	}

    (*nb_frames)++;
  }

  /* FFT exit */
  fft_exit(fft);
  sf_close (infile) ;

  if (!chromas){
    free(chromas);
  }

  return seqs;
}

void
free_sequence(sequence *seqs, int nb_frames)
{
  for (int i = 0; i < nb_frames; i++) {
    free(seqs[i].fft);
    free(seqs[i].chromas);
  }
}

float
cosine_distance(sequence s1, sequence s2)
{
	float product=0;
	float root1=0;
	float root2=0;
	int i;

  int chroma_size = fmin(s1.chroma_size, s2.chroma_size);

	for(i=0; i<chroma_size; i++){
	  product += s1.chromas[i] * s2.chromas[i];
	  root1 += powf(s1.chromas[i], 2);
	  root2 += powf(s2.chromas[i], 2);
	}
	root1 = sqrtf(root1);
	root2 = sqrtf(root2);
  if(root1 == 0 || root2 == 0){
    return 0;
  }
	return product/(root1*root2);
}

float**
calculate_similarity_matrix(char *imageName, sequence *sequence1, int nb_frames1, sequence *sequence2, int nb_frames2)
{
  FILE *fp = fopen(imageName,"w");
  fprintf(fp, "P2\n");
  fprintf(fp, "%d %d\n",nb_frames1,nb_frames2);
  fprintf(fp, "255\n");

  float **M = malloc(nb_frames1*sizeof(float*));
  for(int i=0; i<nb_frames1; i++) M[i] = malloc(nb_frames2*sizeof(float));

  for (int i = 0; i < nb_frames1; i++) {
    for (int j = 0; j < nb_frames2; j++) {
      M[i][j] = cosine_distance(sequence1[i],sequence2[j]);
      fprintf(fp, "%d ", (int)(M[i][j] * 255));
    }
    fprintf(fp, "\n");
  }

  return M;
}

float **
calculate_autosimilarity_matrix(char *imageName, sequence *seq, int nb_frames)
{
  return calculate_similarity_matrix(imageName, seq, nb_frames, seq, nb_frames);
}

double
calculate_tempo(sequence *seq, int nb_frames, int samplerate, int fft_size, int hop_size)
{
  double amplitude[fft_size];
  double amplitude_prev[fft_size];

  double spectralFlux[nb_frames];
  double FS = 0.0;
  double tmp;

  /* init amplitude_prev */
  for(int i = 0; i < fft_size; i++) {
      amplitude_prev[i] = 0.0;
  }

  /* loop through sequences FFT */
  for (int i = 0; i < nb_frames; i++) {

    /* compute SpectralFlux */
    FS = 0.0;
    for(int j = 0; j < fft_size; j++) {
      amplitude_prev[j] = amplitude[j];
      amplitude[j] = cabs( seq[i].fft[j] );

      tmp = amplitude[j] - amplitude_prev[j];
      if(tmp > 0) {
          FS += tmp;
      }
    }

    FS /= (double) fft_size;

    /* threshold detection */
    spectralFlux[i] = (FS > 0.3) ? FS : 0.0;
  }

  double tempo = getTempoEstimation(nb_frames, spectralFlux);
  double tempo_bpm = 60.0 / ((double) tempo * hop_size / samplerate);

  return tempo_bpm;
}


void
calculate_pitch(sequence *seqs, int nb_frames, int *pitch, int *mode)
{
  int histogram[seqs[0].chroma_size];

  /* Init histogram */
  for(int i = 0; i < seqs[0].chroma_size; i++) {
      histogram[i] = 0.0;
  }

  /* Summ all chromas */
  for (int i = 0; i < nb_frames; i++) {
      for (int j = 0; j < seqs[i].chroma_size; j++) {
          histogram[j] = seqs[i].chromas[j];
      }
  }

  /* Actual pitch detection */
  detect_pitch_mode(histogram, pitch, mode);
}
