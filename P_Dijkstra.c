#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int nv, 				// number of vertices
	*notdone, 			// vertices not visited
	nnodes, 			// number of MPI processes in the computation
	chunk, 				// number of vertices handled by each process
	startv,endv, 		// start, end vertices for this process
	rank;

unsigned largeint, 		//	INF
	mymin[2],			// mymin[0] is min for my chunk,
			 			// mymin[1] is vertex which achieves that min

	overallmin[2], 		// overallmin[0] is current min over all nodes,
						// overallmin[1] is vertex which achieves that min
	
	*ohd, 				// 1-hop distances between vertices; "ohd[i][j]" is
						// ohd[i*nv+j]
	*mind; 
	*parent;

double T1,T2; 			// start and finish times
FILE *fin = NULL;

void init(int ac, char **av){
	int i,j,tmp; unsigned u;
	nv = 24000;
	MPI_Init(&ac,&av);
	MPI_Comm_size(MPI_COMM_WORLD,&nnodes);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	chunk = nv/nnodes;			// (number of vertices) / (number of processes)
	startv = rank * chunk;		// 	
	endv = startv + chunk - 1;
	u = -1;
	largeint = u >> 1;
	ohd = malloc(nv*nv*sizeof(int));
	mind = malloc(nv*sizeof(int));
	parent = malloc(nv*sizeof(int));
	notdone = malloc(nv*sizeof(int));
	
	//read grapth from input file
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

// finds closest to 0 among notdone, among startv through endv
void findmymin(){
	int i;
	mymin[0] = largeint;
	for (i = startv; i <= endv; i++){
		if (notdone[i] && mind[i] < mymin[0]) {
			mymin[0] = mind[i];
			mymin[1] = i;
		}
	}
}

// Distribute tasks for each process
void distribute(){ 			// update my mind segment
								// for each i in [startv,endv], ask whether a shorter path to i
								// exists, through mv
	int i, mv = overallmin[1];
	unsigned md = overallmin[0];
	for (i = startv; i <= endv; i++){
		if (md + ohd[mv*nv+i] < mind[i]){
			mind[i] = md + ohd[mv*nv+i];
			parent[i] = mv;
		}
	}
}


void printmind(){ 				// partly for debugging (call from GDB)
	int i;
	printf("minimum distances:\n");
	for (i = 1; i < nv; i++){
		printf("%u    previous vertex  %u\n",mind[i],parent[i]);
	}
}

// 
void dijkstra(){
	int step, 					// index for loop of nv steps
	i;
	if (rank == 0){ 
		T1 = MPI_Wtime();		// start time to excute
	} 
	for (step = 0; step < nv; step++) {		
		findmymin();

		// MPI_Reduce: returns an array of output elements to the root process
		MPI_Reduce(mymin,overallmin,1,MPI_2INT,MPI_MINLOC,0,MPI_COMM_WORLD);
		// MPI_Bcast: one process sends the same data to all processes in a communicator
		MPI_Bcast(overallmin,1,MPI_2INT,0,MPI_COMM_WORLD);
		
		// mark new vertex as done
		notdone[overallmin[1]] = 0;
		distribute();
	}
	// now need to collect all the mind values from other nodes to node 0
	MPI_Gather(mind+startv,chunk,MPI_INT,mind,chunk,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Gather(parent+startv,chunk,MPI_INT,parent,chunk,MPI_INT,0,MPI_COMM_WORLD);
	T2 = MPI_Wtime();			// finish time to excute
}

int main(int argc, char **argv){
	int i,j,print;
	init(argc,argv);
	dijkstra();
	print = 0; 					// just print when running small test
	if (print == 1 && rank == 0) {
		printf("graph weights:\n");
		for (i = 0; i < nv; i++) {
			for (j = 0; j < nv; j++){
				printf("%u ",ohd[nv*i+j]);
			}
			printf("\n");
		}
		printmind();
	}
	if (rank == 0){
		printf("time at node 0: %f\n",(float)(T2-T1));
	}
	MPI_Finalize();
}