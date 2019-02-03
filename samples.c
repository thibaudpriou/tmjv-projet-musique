#include "samples.h"

void
fill_buffer(double *buffer, double *new_buffer, int fft_size, int hop_size)
{
  int i;
  double tmp[fft_size - hop_size];

  /* save */
  for (i=0; i<fft_size - hop_size; i++){
    tmp[i] = buffer[i+hop_size];
  }

  /* save offset */
  for (i=0; i<(fft_size-hop_size); i++) {
    buffer[i] = tmp[i];
  }

  for (i=0; i<hop_size; i++) {
    buffer[fft_size - hop_size + i] = new_buffer[i];
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
read_samples (SNDFILE * infile, double * buffer, int channels, int hop_size)
{
  return read_n_samples (infile, buffer, channels, hop_size);
}

int
write_samples (SNDFILE * outfile, double * buffer, int channels, int hop_size)
{
  return write_n_samples (outfile, buffer, channels, hop_size);
}
