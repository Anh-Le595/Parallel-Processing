#include <stdio.h>
#include <stdlib.h>
#include "time.h"
int nv = 24000; // number of vertices
unsigned largeint, // max possible unsigned int
	*ohd; // 1-hop distances between vertices; "ohd[i][j]" is
int i,j;

int main(){
	ohd = (unsigned int*)malloc(nv*nv*sizeof(int));

	srand(time(NULL));
	for (i = 0; i < nv; i++){
		for (j = i; j < nv; j++) {
			if (j == i){
				ohd[i*nv + i] = 0;
			}
			else {
				ohd[nv*i + j] = rand() % 20;
				ohd[nv*j + i] = ohd[nv*i + j];
			}
		}
	}

	FILE *f = fopen("input.txt", "w");
	for (i = 0; i < nv; i++){
		for (j = 0; j < nv; j++) {
			fprintf(f, "%d ", ohd[nv*i + j]);
		}
	}
	fclose(f);
	return 0;
}