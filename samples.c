#include "samples.h"

void
fill_buffer(double *buffer, double *new_buffer)
{
  int i;
  double tmp[FRAME_SIZE-HOP_SIZE];

  /* save */
  for (i=0; i<FRAME_SIZE-HOP_SIZE; i++){
    tmp[i] = buffer[i+HOP_SIZE];
  }

  /* save offset */
  for (i=0; i<(FRAME_SIZE-HOP_SIZE); i++) {
    buffer[i] = tmp[i];
  }

  for (i=0;i<HOP_SIZE;i++) {
    buffer[FRAME_SIZE-HOP_SIZE+i] = new_buffer[i];
  }
}

int
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

int
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


int
read_samples (SNDFILE * infile, double * buffer, int channels)
{
  return read_n_samples (infile, buffer, channels, HOP_SIZE);
}

int
write_samples (SNDFILE * outfile, double * buffer, int channels)
{
  return write_n_samples (outfile, buffer, channels, HOP_SIZE);
}
