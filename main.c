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
  printf("\n Usage : %s <input file> <output file> <print>(opt)\n", progname);
  puts("\n");
}

int
main(int argc, char * argv [])
{
  char 		*progname, *infilename, *outfilename;
  int print = 0;
  SNDFILE	 	*infile = NULL;
  SNDFILE		*outfile = NULL;
  SF_INFO	 	sfinfo;

  fftw_plan plan = NULL;
  gnuplot_ctrl *h;

  progname = strrchr(argv[0], '/');
  progname = progname ? progname + 1 : argv[0];
	if(argc != 3 && argc != 4) {

    print_usage(progname);
    return 1;
  }

  infilename = argv[1];
  outfilename = argv[2];
  if(argc == 4){
    print = atoi(argv[3]);
  }

  int nb_frames_seq = 0;
  sequence * seq = fill_sequence(infilename, &nb_frames_seq, CHROMA_SIZE);

  if (seq == NULL){
    printf("erreur calcul sequence \n");
    return 1;
  }

	if(strcmp(infilename, outfilename) == 0) {
    printf("Error : Input and output filenames are the same.\n\n");
    print_usage(progname);
    return 1;
  }

  if((infile = sf_open(infilename, SFM_READ, &sfinfo)) == NULL) {
    printf("Not able to open input file %s.\n", infilename);
    puts(sf_strerror(NULL));
    return 1;
  }

  int nb_frames_in = (int) sfinfo.frames/HOP_SIZE;

  /* Open the output file. */
  if((outfile = sf_open(outfilename, SFM_WRITE, &sfinfo)) == NULL) {
    printf("Not able to open output file %s.\n", outfilename);
    puts(sf_strerror(NULL));
    return 1;
  }

  /* Read WAV */
  int nb_frames = 0;
  double new_buffer[HOP_SIZE];
  double buffer[FRAME_SIZE];

  int i;
  for(i=0; i<(FRAME_SIZE/HOP_SIZE-1); i++) {
    if(read_samples(infile, new_buffer, sfinfo.channels) == 1) {
			fill_buffer(buffer, new_buffer);
    } else {
      printf("not enough samples !!\n");
      return 1;
    }
  }


  complex samples[FRAME_SIZE];
  double amplitude[FRAME_SIZE];
  double amplitude_prev[FRAME_SIZE];
  complex spec[FRAME_SIZE];
  double spectralFlux[nb_frames_in];
  double FS = 0.0;

  for(i = 0; i < FRAME_SIZE; i++) {
      amplitude_prev[i] = 0.0;
  }

  /* Plot init */
  h = gnuplot_init();
  gnuplot_setstyle(h, "lines");

  /* FFT init */
  plan = fft_init(samples, spec);

  while(read_samples(infile, new_buffer, sfinfo.channels) == 1) {
    /* Process Samples */
    printf("Processing frame %d\n",nb_frames);

    /* hop size */
    fill_buffer(buffer, new_buffer);

    // fft input
    for(i = 0; i < FRAME_SIZE; i++) {
      samples[i] = buffer[i];
    }

    fft_process(plan);

    // amplitude
    for(i = 0; i < FRAME_SIZE; i++) {
      amplitude_prev[i] = amplitude[i];
      amplitude[i] = cabs(spec[i]);
    }

		/* PLOT */
		// gnuplot_resetplot(h);
		// gnuplot_plot_x(h, amplitude, FRAME_SIZE, "FFT");
		// sleep(1);
		double chromas[NB_NOTES_OCTAVE];

    if(print == 1){
      gnuplot_resetplot(h);
      computeChromas(spec, FRAME_SIZE, sfinfo.samplerate, chromas);
      gnuplot_plot_x(h, chromas, NB_NOTES_OCTAVE, "Chromas");
      sleep(1);
    }

    /* SPECTRAL FLUX !!! */
    FS = 0.0;

    for(i = 0; i < FRAME_SIZE; i++) {
      double tmp = amplitude[i] - amplitude_prev[i];
      if(tmp > 0) {
          FS += tmp;
      }
    }

    FS /= (double)FRAME_SIZE;

    /* threshold detection */
    if(FS > 0.3) {
      spectralFlux[nb_frames] = FS;

      /* sin synthesis */
      for(i = 0; i < FRAME_SIZE; i++) {
        buffer[i] +=(0.25*sin(2*M_PI*1760.0*(double)i/sfinfo.samplerate)) *
       			(0.5-0.5*cos(2.0*M_PI*(double)i/FRAME_SIZE));;
      }
    } else {
      spectralFlux[nb_frames] = 0.0;
    }

    /* SAVE */
    // if(write_samples(outfile, buffer, sfinfo.channels)!=1){
    //   printf("saving problem !! \n");
    // }

    nb_frames++;
  }


  /* PLOT */
  if(print == 1){
    gnuplot_resetplot(h);
    gnuplot_plot_x(h,spectralFlux,nb_frames,"Beats");
    sleep(2);
  }

  /* TEMPO ESTIMATION */
  int imax = getTempoEstimation(nb_frames_in,spectralFlux);

  printf("max autocorrelation %d\n", imax);
  printf("période en seconde %f\n", (double)imax * HOP_SIZE / sfinfo.samplerate);
  printf("Fréquence en Hz %f\n", sfinfo.samplerate / ((double)imax*HOP_SIZE));
  printf("Tempo en BPM %f\n", 60.0 / ((double)imax * HOP_SIZE / sfinfo.samplerate));

  /* PLOT */
  /*
  gnuplot_resetplot(h);
  gnuplot_plot_x(h,gamma,300,"Autocorrelation");
  sleep(20);
  */

  /* ---------------- */
  // Calcul du dephasage
  double peigne[nb_frames_in];
  for(int i=0; i < nb_frames_in; i++) {
    if(i % imax == 0) {
    	peigne[i] = 1;
    } else{
    	peigne[i] = 0;
		}
  }

  double gamma[nb_frames_in];
  for(int tau = 0; tau < nb_frames_in; tau++) {
    gamma[tau]=0;
    for(int i = 0; i < nb_frames_in-tau; i++) {
	    gamma[tau] += (spectralFlux[i] * peigne[i+tau]) / (double)nb_frames_in;
	    //gamma[tau]/=(1.0/nb_frames);
		}
  }

  int peigne_imax=0;
  double peigne_max = 0.0;

  for(int i=0; i < imax; i++){
	  if(gamma[i] > peigne_max) {
	    peigne_max = gamma[i];
	    peigne_imax = i;
	  }
	}

  printf("max intercorrelation %d\n",peigne_imax);
  /* PLOT */
  if(print == 1){
    gnuplot_resetplot(h);
    gnuplot_plot_x(h,gamma,nb_frames_in,"Peigne");
    sleep(2);
  }

  // sf_close(infile);

  /* Synthesis beat tracker */
  if((infile = sf_open(infilename, SFM_READ, &sfinfo)) == NULL)
  {
    printf("Not able to open input file %s.\n", infilename);
    puts(sf_strerror(NULL));
    return 1;
  };

  /* Open the output file. */
  if((outfile = sf_open(outfilename, SFM_WRITE, &sfinfo)) == NULL)
  {
    printf("Not able to open output file %s.\n", outfilename);
    puts(sf_strerror(NULL));
    return 1;
  };

  for(i=0;i<(FRAME_SIZE/HOP_SIZE-1);i++)
  {
    if(read_samples(infile, new_buffer, sfinfo.channels)==1)
    fill_buffer(buffer, new_buffer);
    else
    {
      printf("not enough samples !!\n");
      return 1;
    }
  }

  nb_frames=0;

  while(read_samples(infile, new_buffer, sfinfo.channels)==1)
  {
    /* Process Samples */
    printf("Step 2 : Processing frame %d\n",nb_frames);
    /* hop size */
    fill_buffer(buffer, new_buffer);

    if(nb_frames % imax ==(imax - peigne_imax)) /* beat located */
    {
      /* sin synthesis */

      for(i = 0; i < FRAME_SIZE; i++)
      {
        buffer[i] +=(0.5*sin(2*M_PI*1760.0*(double)i/sfinfo.samplerate)) *
       (0.5-0.5*cos(2.0*M_PI*(double)i/FRAME_SIZE));;
      }
    }

    /* SAVE */
    if(write_samples(outfile, buffer, sfinfo.channels)!=1){
      printf("saving problem !! \n");
    }
    nb_frames++;
  }

  calculate_autosimilarity_matrix("image.ppm",seq,nb_frames_seq);


  sf_close(infile);
  sf_close(outfile);

  /* FFT exit */
  fft_exit(plan);

  return 0;
} /* main */
