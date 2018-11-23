#include "rythme.h"

int tempo(int nb_frames,double *data){
	double gamma[nb_frames];

  for(int tau = 0; tau < nb_frames; tau++)
  {
    gamma[tau]=0;
    for(int i = 0; i < nb_frames-tau; i++)
    gamma[tau] +=(data[i]*data[i+tau])/(double)nb_frames;
  }

  int imax=0;
  double autoc_max = 0.0;

  for(int i=20; i < 200; i++) //nb_frames //200 pour NiN
  if(gamma[i] > autoc_max)
  {
    autoc_max = gamma[i];
    imax = i;
  }
	return imax;
}
