#include "sequence.h"

sequence* create_sequence(int size){
  sequence *s = malloc(sizeof(sequence));
  s->size = size;
  s->data = malloc(size * sizeof(double));
  return s;
}

float cosine_distance(sequence s1, sequence s2){
	float product=0;
	float root1=0;
	float root2=0;
	int i;

  int size = min(s1.size,s2.size);

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
