#include <stdio.h>
#include <stdlib.h>

#define N 10000000
#define K 4

// token shaggy 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

typedef struct point {
    float x;
    float y;
    int cluster;
} Point;

typedef struct cluster {
    float x;
    float y;
} Cluster;

Point *points __attribute__((aligned (32)));
Cluster *centroids __attribute__((aligned (32)));
int *n_points __attribute__((aligned (32)));
int has_converged;

void aloca() {
    points = malloc(sizeof(struct point) * N);
    centroids = malloc(sizeof(struct cluster) * K);
    n_points = malloc(sizeof(int) * K);
    has_converged = 0;
}

void inicializa() {
    srand(10);
    for(int i = 0; i < N; i++) {
        points[i].x = (float) rand() / RAND_MAX;
        points[i].y = (float) rand() / RAND_MAX;
    }
    for(int i = 0; i < K; i++) {
        centroids[i].x = points[i].x;
        centroids[i].y = points[i].y;
    }
}

void cluster_points() { 
    float sum_x[K] __attribute__((aligned (32)));
    float sum_y[K] __attribute__((aligned (32)));
    
    for (int i = 0; i < K; i++) {
        sum_x[i] = 0; 
        sum_y[i] = 0; 
        n_points[i] = 0; 
    }

    for (int j = 0; j < N; j++) {
        float dist = (points[j].x - centroids[0].x)*(points[j].x - centroids[0].x) + (points[j].y - centroids[0].y)*(points[j].y - centroids[0].y);
        points[j].cluster = 0;

        for (int i = 1; i < K; i++) {        
            float tmp = (points[j].x - centroids[i].x)*(points[j].x - centroids[i].x) + (points[j].y - centroids[i].y)*(points[j].y - centroids[i].y);
            if (tmp < dist) {
                points[j].cluster = i;
                dist = tmp;
            }
        }

        n_points[points[j].cluster] += 1;
        sum_x[points[j].cluster] += points[j].x;
        sum_y[points[j].cluster] += points[j].y;
    }

    has_converged = 1;
    for (int i = 0; i < K; i++) {
        float new_x = sum_x[i] / n_points[i];
        float new_y = sum_y[i] / n_points[i];
        if (centroids[i].x != new_x || centroids[i].y != new_y)
            has_converged = 0;
        centroids[i].x = new_x;
        centroids[i].y = new_y;
    }
}

int main() {
    int iterations = -1;

    aloca();

    inicializa();

    while (!has_converged) {
        cluster_points();
        iterations++;
    }

    printf("N = %d, K = %d\n", N, K);
    for (int i = 0; i < K; i++)
        printf("Center: (%.3f, %.3f) : Size: %d\n", centroids[i].x, centroids[i].y, n_points[i]);
    printf("Iterations: %d\n", iterations);

    free(points);
    free(centroids);

    return 0;
}
