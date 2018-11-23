#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <lib/sndfile.h>

#include <math.h>
#include <complex.h>
#include <lib/fftw3.h>
#include "lib/gnuplot_i.h"


#define	FRAME_SIZE 512
#define HOP_SIZE 256

static gnuplot_ctrl *h;
static fftw_plan plan = NULL;

static void
print_usage (char *progname)
{	printf ("\nUsage : %s <input file> <output file>\n", progname) ;
puts ("\n"
) ;

}
static void
fill_buffer(double *buffer, double *new_buffer)
{
	int i;
	double tmp[FRAME_SIZE-HOP_SIZE];

	/* save */
	for (i=0;i<FRAME_SIZE-HOP_SIZE;i++)
	tmp[i] = buffer[i+HOP_SIZE];

	/* save offset */
	for (i=0;i<(FRAME_SIZE-HOP_SIZE);i++)
	{
		buffer[i] = tmp[i];
	}

	for (i=0;i<HOP_SIZE;i++)
	{
		buffer[FRAME_SIZE-HOP_SIZE+i] = new_buffer[i];
	}
}

static int
read_n_samples (SNDFILE * infile, double * buffer, int channels, int n)
{

	if (channels == 1)
	{
		/* MONO */
		int readcount ;

		readcount = sf_readf_double (infile, buffer, n);

		return readcount==n;
	}
	else if (channels == 2)
	{
		/* STEREO */
		double buf [2 * n] ;
		int readcount, k ;
		readcount = sf_readf_double (infile, buf, n);
		for (k = 0 ; k < readcount ; k++)
		buffer[k] = (buf [k * 2]+buf [k * 2+1])/2.0 ;

		return readcount==n;
	}
	else
	{
		/* FORMAT ERROR */
		printf ("Channel format error.\n");
	}

	return 0;
}

static int
write_n_samples (SNDFILE * outfile, double * buffer, int channels, int n)
{
	if (channels == 1)
	{
		/* MONO */
		int writecount ;

		writecount = sf_writef_double (outfile, buffer, n);

		return writecount==n;
	}
	else
	{
		/* FORMAT ERROR */
		printf ("Channel format output error.\n");
	}

	return 0;
}


static int
read_samples (SNDFILE * infile, double * buffer, int channels)
{
	return read_n_samples (infile, buffer, channels, HOP_SIZE);
}

static int
write_samples (SNDFILE * outfile, double * buffer, int channels)
{
	return write_n_samples (outfile, buffer, channels, HOP_SIZE);
}


void
fft_init (complex in[FRAME_SIZE], complex spec[FRAME_SIZE])
{
	plan = fftw_plan_dft_1d (FRAME_SIZE, in, spec, FFTW_FORWARD, FFTW_ESTIMATE);
}

void
fft_exit (void)
{
	fftw_destroy_plan (plan);
}

void
fft_process (void)
{
	fftw_execute (plan);
}


int
main (int argc, char * argv [])
{	char 		*progname, *infilename, *outfilename ;
	SNDFILE	 	*infile = NULL ;
	SNDFILE		*outfile = NULL ;
	SF_INFO	 	sfinfo ;

	progname = strrchr (argv [0], '/') ;
	progname = progname ? progname + 1 : argv [0] ;

	if (argc != 3)
	{	print_usage (progname) ;
		return 1 ;
	} ;

	infilename = argv [1] ;
	outfilename = argv [2] ;

	if (strcmp (infilename, outfilename) == 0)
	{	printf ("Error : Input and output filenames are the same.\n\n") ;
	print_usage (progname) ;
	return 1 ;
} ;


if ((infile = sf_open (infilename, SFM_READ, &sfinfo)) == NULL)
{	printf ("Not able to open input file %s.\n", infilename) ;
puts (sf_strerror (NULL)) ;
return 1 ;
} ;

/* Number of frames */
// int nb_frames_in = (int)sfinfo.frames/HOP_SIZE;

/* Open the output file. */
if ((outfile = sf_open (outfilename, SFM_WRITE, &sfinfo)) == NULL)
{	printf ("Not able to open input file %s.\n", outfilename) ;
puts (sf_strerror (NULL)) ;
return 1 ;
} ;

/* Read WAV */
int nb_frames = 0;
double new_buffer[HOP_SIZE];
double buffer[FRAME_SIZE];

int i;
for (i=0;i<(FRAME_SIZE/HOP_SIZE-1);i++)
{
	if (read_samples (infile, new_buffer, sfinfo.channels)==1)
	fill_buffer(buffer, new_buffer);
	else
	{
		printf("not enough samples !!\n");
		return 1;
	}
}

complex samples[FRAME_SIZE];
double amplitude[FRAME_SIZE];
complex spec[FRAME_SIZE];

/* Plot init */
h=gnuplot_init();
gnuplot_setstyle(h, "lines");

/* FFT init */
fft_init(samples, spec);

while (read_samples (infile, new_buffer, sfinfo.channels)==1)
{
	/* Process Samples */
	printf("Processing frame %d\n",nb_frames);

	/* hop size */
	fill_buffer(buffer, new_buffer);

	// fft input
	for (i = 0; i < FRAME_SIZE; i++)
	samples[i] = buffer[i];

	fft_process();

	// amplitude
	for (i = 0; i < FRAME_SIZE; i++)
	{
		amplitude[i] = cabs(spec[i]);
	}

	/* Detect Beats */
	/* TODO */

	/* PLOT */
	gnuplot_resetplot(h);
	gnuplot_plot_x(h,amplitude,FRAME_SIZE/2,"Amplitude");
	sleep(1);

	/* SAVE */
	if (write_samples (outfile, buffer, sfinfo.channels)!=1)
	printf("saving problem !! \n");


	nb_frames++;
}


/* Plot Beats */
/* TODO */

sf_close (infile) ;
sf_close (outfile) ;

/* FFT exit */
fft_exit();

return 0 ;
} /* main */
