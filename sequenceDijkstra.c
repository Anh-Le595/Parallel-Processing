#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int nv= 24000, // number of vertices
*notdone; // vertices not checked yet
unsigned largeint, // max possible unsigned int
*ohd, // 1-hop distances between vertices; "ohd[i][j]" is
// ohd[i*nv+j]
*mind, // min distances found so far
*parent;
clock_t   T1, T2; // start and finish times
int i, j;
FILE *fin = NULL;
void init(){
	 
	unsigned u;
	u = -1;
	largeint = u >> 1;
	ohd = (int*) malloc(nv*nv*sizeof(int));
	mind = (int*)malloc(nv*sizeof(int));
	parent = (int*)malloc(nv*sizeof(int));
	notdone = (int*)malloc(nv*sizeof(int));
	fin = fopen("input.txt","r");
	for (i = 0; i < nv; i++){
		for (j = 0; j < nv; j++) {
			fscanf(fin, "%d", &ohd[nv*i + j]);
		}
	}

	fclose(fin);

	for (i = 0; i < nv; i++) {
		notdone[i] = 1;
		mind[i] = largeint;
	}
	mind[0] = 0;
	parent[0] = 0;
}

void Dijkstra(){
	int loop;
	for (loop = 0; loop < nv; loop++){// loop nv times for nv node notdone
		unsigned tmpmin = largeint;
		int tmpminNode = 0;
		for (i = 0; i < nv; i++){
			if (notdone[i] && mind[i] < tmpmin){
				tmpmin = mind[i];
				tmpminNode = i;
			}
		}
		notdone[tmpminNode] = 0;
		for (j = 0; j < nv; j++){
			if (notdone[j] && ohd[tmpminNode*nv + j] + tmpmin < mind[j]){
				parent[j] = tmpminNode;
				mind[j] = ohd[tmpminNode*nv + j] + tmpmin;
			}
		}
	}
}


int main(){
	init();
	/*printf("graph weights:\n");
	for (i = 0; i < nv; i++) {
		for (j = 0; j < nv; j++){
			printf("%u ", ohd[nv*i + j]);
		}
		printf("\n");
	}*/
	T1 = clock();
	Dijkstra();
	T2 = clock();

	double extime = (double)(T2 - T1) / CLOCKS_PER_SEC;
	printf("\n\tExecution time for the Dijkstra is %f seconds\n ", extime);
	printf("\n");
	/*for (i = 1; i < nv; i++){
		printf("%u    previous vertex  %u\n", mind[i], parent[i]);
	}*/
}