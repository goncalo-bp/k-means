#include <stdio.h>
#include <stdlib.h>

#define N 10000000
#define K 4

// token shaggy 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

typedef struct point {
    float x;
    float y;
} Point;

Point *points __attribute__((aligned (32)));
Point *centroids __attribute__((aligned (32)));
int *n_points __attribute__((aligned (32)));
float *new_x __attribute__((aligned (32)));
float *new_y __attribute__((aligned (32)));
int has_converged;

void aloca() {
    points = malloc(sizeof(struct point) * N);
    centroids = malloc(sizeof(struct point) * K);
    n_points = malloc(sizeof(int) * K);
    new_x = malloc(sizeof(float) * K);
    new_y = malloc(sizeof(float) * K);
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
    int cluster_id;
    for (int i = 0; i < K; i++) {
        new_x[i] = 0; 
        new_y[i] = 0; 
        n_points[i] = 0; 
    }

    for (int j = 0; j < N; j++) {
        float dist = (points[j].x - centroids[0].x)*(points[j].x - centroids[0].x) + (points[j].y - centroids[0].y)*(points[j].y - centroids[0].y);
        cluster_id = 0;
        for (int i = 1; i < K; i++) {        
            float tmp = (points[j].x - centroids[i].x)*(points[j].x - centroids[i].x) + (points[j].y - centroids[i].y)*(points[j].y - centroids[i].y);
            if (tmp < dist) {
                cluster_id = i;
                dist = tmp;
            }
        }

        n_points[cluster_id] += 1;
        new_x[cluster_id] += points[j].x;
        new_y[cluster_id] += points[j].y;
    }

    has_converged = 1;
    
    for (int i = 0; i < K; i++) {
        new_x[i] = new_x[i] / n_points[i];
        new_y[i] = new_y[i] / n_points[i];
    }
    for (int i = 0; i < K; i++) {
        if (centroids[i].x != new_x[i] || centroids[i].y != new_y[i])
            has_converged = 0;
    }
    for (int i = 0; i < K; i++) {
        centroids[i].x = new_x[i];
        centroids[i].y = new_y[i];
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
    free(new_x);
    free(new_y);
    free(n_points);

    return 0;
}
