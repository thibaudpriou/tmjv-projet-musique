#include "sequence.h"

sequence init_sequence(int size){
  sequence *s = malloc(sizeof(struct sequence));
  s->size = size;
  s->data = malloc(size * sizeof(double));
  return *s;
}

sequence* fill_sequence(char* infilename, int *nb_frames, int size)
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
  double new_buffer[HOP_SIZE];
  double buffer[FRAME_SIZE];

  int i;
  for (i=0;i<(FRAME_SIZE/HOP_SIZE-1);i++){
    if (read_samples (infile, new_buffer, sfinfo.channels)==1){
    	fill_buffer(buffer, new_buffer);
    } else {
  	  printf("not enough samples !!\n");
  	  return NULL;
  	}
  }

  printf("taille : %d échantillons \n", (int)sfinfo.frames);
  int seq_size = (int)ceil((double)sfinfo.frames/HOP_SIZE);
  printf("nbre symboles : %d \n", seq_size);

  sequence* sequence1 = malloc(sizeof(struct sequence)*seq_size);
  //assert(sequence1);

  complex samples[FRAME_SIZE];
  double amplitude[FRAME_SIZE];
  complex spec[FRAME_SIZE];

  const int chroma_size = size;
  double *chroma = malloc(sizeof(double)*chroma_size);
  for (i = 0; i < chroma_size; i++){
    chroma[i] = 0;
  }

  /* FFT init */
  fftw_plan fft = fft_init(samples, spec);

  while (read_samples (infile, new_buffer, sfinfo.channels)==1){
      sequence1[(*nb_frames)] = init_sequence(chroma_size);
      /* Process Samples */
      //printf("Processing frame %d\n",nb_frames);

      /* hop size */
      fill_buffer(buffer, new_buffer);

      // fft input
      for (i = 0; i < FRAME_SIZE; i++){
      	samples[i] = buffer[i];
      }

      fft_process(fft);

      // amplitude
      for (i = 0; i < FRAME_SIZE; i++){
    	  amplitude[i] = cabs(spec[i]);
    	}

      /* ---- */
      // contribute chroma
      for (i = 0; i < FRAME_SIZE/2; i++){
    	  int bin_chroma = my_modulo(freq2chro((double)i*sfinfo.samplerate/(double)FRAME_SIZE,chroma_size),chroma_size);
    	  chroma[bin_chroma] += amplitude[i];
    	}

      for (i = 0; i < chroma_size; i++){
    	  sequence1[(*nb_frames)].data[i] = chroma[i];
    	}

      /* reset chroma */
      for (i = 0; i < chroma_size; i++){
      	chroma[i] = 0;
      }

      (*nb_frames)++;
    }

  /* FFT exit */
  fft_exit(fft);
  sf_close (infile) ;

  if (!chroma){
    free(chroma);
  }

  return sequence1;
}

float cosine_distance(sequence s1, sequence s2){
	float product=0;
	float root1=0;
	float root2=0;
	int i;

  int size = fmin(s1.size,s2.size);

	for(i=0;i<size;i++){
	  product += s1.data[i]*s2.data[i];
	  root1 += powf(s1.data[i],2);
	  root2 += powf(s2.data[i],2);
	}
	root1 = sqrtf(root1);
	root2 = sqrtf(root2);
  if(root1 == 0 || root2 == 0){
    return 0;
  }
	return product/(root1*root2);
}

float** calculate_similarity_matrix(char *imageName, sequence *sequence1, int nb_frames1, sequence *sequence2, int nb_frames2){
  FILE *fp = fopen(imageName,"w");
  fprintf(fp, "P2\n");
  fprintf(fp, "%d %d\n",nb_frames1,nb_frames2);
  fprintf(fp, "255\n");

  float **M = malloc(nb_frames1*sizeof(float*));
  for(int i=0;i<nb_frames1;i++) M[i] = malloc(nb_frames2*sizeof(float));

  for (int i = 0; i < nb_frames1; i++) {
    for (int j = 0; j < nb_frames2; j++) {
      M[i][j] = cosine_distance(sequence1[i],sequence2[j]);
      fprintf(fp, "%d ", (int)(M[i][j] * 255));
    }
    fprintf(fp, "\n");
  }

  return M;
}

float ** calculate_autosimilarity_matrix(char *imageName, sequence *seq, int nb_frames){
  return calculate_similarity_matrix(imageName, seq, nb_frames, seq, nb_frames);
}
