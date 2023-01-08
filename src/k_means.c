#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <stdarg.h>

#define MAX_ITERATIONS 20

int nClusters = 0;
int nPoints = 0;
int nProcs = 0;

// Inicializa os pontos com valores aleatóris e os centroides com os valores dos primeiros K pontos
void init(float *data_x, float *data_y, float *k_means_x, float *k_means_y) {
    srand(10);
    for(int i = 0; i < nPoints; i++) {
        data_x[i] = (float) rand() / RAND_MAX;
        data_y[i] = (float) rand() / RAND_MAX;
    }
    for(int i = 0; i < nClusters; i++) {
        k_means_x[i] = data_x[i];
        k_means_y[i] = data_y[i];
    }
}

// Calcula a distância entre dois pontos
float distance(float p1_x, float p1_y, float p2_x, float p2_y) {
    return (p1_x - p2_x) * (p1_x - p2_x) + (p1_y - p2_y) * (p1_y - p2_y);
}

/* This function goes through that data points and assigns them to a cluster */
void assign_cluster(float *k_x, float *k_y, float *recv_x, float *recv_y, float *sum_x, float *sum_y, int *k_points) {
	int index;
	float min_dist, temp_dist;

	for(int i = 0; i < nPoints/nProcs; i++) {
		min_dist = distance(k_x[0], k_y[0], recv_x[i], recv_y[i]);
		index = 0;

		for(int j = 1; j < nClusters; j++) {
			temp_dist = distance(k_x[j], k_y[j], recv_x[i], recv_y[i]);

			// new minimum distance found
			if(temp_dist < min_dist) {
				min_dist = temp_dist;
				index = j;
			}
		}

		sum_x[index] += recv_x[i];
		sum_y[index] += recv_y[i];
		k_points[index]++;
	}
}

int main(int argc, char **argv) {
    // initialize the MPI environment
	MPI_Init(&argc, &argv);

	// get number of processes
	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	nProcs = size;

	// get rank
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // send buffers
	float *k_means_x = NULL;		// k means corresponding x values
	float *k_means_y = NULL;		// k means corresponding y values

	int *k_points = NULL;			// number of points assigned to each cluster
	int *recv_k_points = NULL;

	float *sum_x = NULL;
	float *sum_y = NULL;
	float *res_x = NULL;
	float *res_y = NULL;

	float *data_x_points = NULL;
	float *data_y_points = NULL;
	float *recv_x = NULL;
	float *recv_y = NULL;


	if (argc < 2) {
        printf("Número argumentos: %d\n", argc);
        printf("Argumentos insuficientes: ./k_means <Num_Points> <Num_Clusters>\n");
        exit(-1);
    }
    nPoints = atoi(argv[1]);
    nClusters = atoi(argv[2]);

	// allocate memory for arrays
	k_means_x = (float *)malloc(sizeof(float) * nClusters);
	k_means_y = (float *)malloc(sizeof(float) * nClusters);
	if(k_means_x == NULL || k_means_y == NULL) {
		perror("malloc");
		exit(-1);
	}

	// allocate memory for an array of data points
	data_x_points = (float *)malloc(sizeof(float) * nPoints);
	data_y_points = (float *)malloc(sizeof(float) * nPoints);
	if(data_x_points == NULL || data_y_points == NULL) {
		perror("malloc");
		exit(-1);
	}

	if (rank == 0){
		init(data_x_points, data_y_points, k_means_x, k_means_y);

		printf("\nRunning k-means algorithm for %d iterations...\n\n", MAX_ITERATIONS);
		for(int i = 0; i < nClusters; i++) {
			printf("Initial K-means: (%f, %f)\n", k_means_x[i], k_means_y[i]);
		}
	}

	// allocate memory for receive buffers
	recv_x = (float *) malloc(sizeof(float) * (nPoints/nProcs));
	recv_y = (float *) malloc(sizeof(float) * (nPoints/nProcs));
	if(recv_x == NULL || recv_y == NULL) {
		perror("malloc");
		exit(-1);
	}


	k_points = (int *)malloc(sizeof(int) * nClusters);
	recv_k_points = (int *)malloc(sizeof(int) * nClusters);
	sum_x = (float *)malloc(sizeof(float) * nClusters);
	sum_y = (float *)malloc(sizeof(float) * nClusters);
	res_x = (float *)malloc(sizeof(float) * nClusters);
	res_y = (float *)malloc(sizeof(float) * nClusters);
	for(int i = 0; i < nClusters; i++) {
		k_points[i] = 0;
		sum_x[i] = 0;
		sum_y[i] = 0;
    }

	/* Distribute the work among all nodes. The data points itself will stay constant and
	   not change for the duration of the algorithm. */
	MPI_Scatter(data_x_points, (nPoints/nProcs), MPI_FLOAT, recv_x, (nPoints/nProcs), MPI_FLOAT, 0, MPI_COMM_WORLD);
	MPI_Scatter(data_y_points, (nPoints/nProcs), MPI_FLOAT, recv_y, (nPoints/nProcs), MPI_FLOAT, 0, MPI_COMM_WORLD);

	int it = 0;
	while(it < MAX_ITERATIONS) {
		// update k-means on all processes from root data
		MPI_Bcast(k_means_x, nClusters, MPI_FLOAT, 0, MPI_COMM_WORLD);
		MPI_Bcast(k_means_y, nClusters, MPI_FLOAT, 0, MPI_COMM_WORLD);

		// assign the data points to a cluster
		assign_cluster(k_means_x, k_means_y, recv_x, recv_y, sum_x, sum_y, k_points);

		// gather information
		MPI_Reduce(sum_x, res_x, 4, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
		MPI_Reduce(sum_y, res_y, 4, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
		MPI_Reduce(k_points, recv_k_points, 4, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

		// let the root process recalculate k means
		if(rank == 0) {
			printf("--------------------------------------------------\n");
            for(int i = 0; i < nClusters; i++) {
				k_means_x[i] = res_x[i] / recv_k_points[i];
				k_means_y[i] = res_y[i] / recv_k_points[i];
				printf("Center #%d: (%.3f, %.3f) : Size: %d\n", i, k_means_x[i], k_means_y[i], recv_k_points[i]);
		    }
            printf("Iterations: %d\n", it+1);
			printf("--------------------------------------------------\n");
		}

		for (int i = 0; i < nClusters; i++) {
			sum_x[i] = 0;
			sum_y[i] = 0;
			k_points[i] = 0;
		}

		it++;
	}

	// deallocate memory and clean up
	free(k_means_x);
	free(k_means_y);
	free(data_x_points);
	free(data_y_points);
	free(k_points);
	free(recv_k_points);
	free(recv_x);
	free(recv_y);
	free(sum_x);
	free(sum_y);
	free(res_x);
	free(res_y);

	//MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
}
