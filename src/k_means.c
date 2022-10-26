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

Point *points;
Cluster *centroids;
Cluster *old_centroids;
int *n_points;

void aloca() {
    points = malloc(sizeof(struct point) * N);
    centroids = malloc(sizeof(struct cluster) * K);
    old_centroids = malloc(sizeof(struct cluster) * K);
    n_points = malloc(sizeof(int) * K);
}

float distance(float x1, float y1, float x2, float y2) {
    return (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2);
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
    for (int j = 0; j < N; j++) {
        float dist = distance(points[j].x, points[j].y , centroids[0].x, centroids[0].y);
        points[j].cluster = 0;
        for (int i = 1; i < K; i++) {
            float tmp = distance(points[j].x, points[j].y , centroids[i].x, centroids[i].y);
            if (tmp < dist) {
                points[j].cluster = i;
                dist = tmp;
            }
        }
    }
}

void reevaluate_points() {
    float sum_x[K], sum_y[K];

    for (int i = 0; i < K; i++) {
        sum_x[i] = 0;
        sum_y[i] = 0;
        n_points[i] = 0;
    }

    #pragma GCC unroll 4
    for (int i = 0; i < N; i++) {
        sum_x[points[i].cluster] += points[i].x;
        sum_y[points[i].cluster] += points[i].y;
        n_points[points[i].cluster] += 1;
    }

    for (int i = 0; i < K; i++) {
        centroids[i].x = sum_x[i] / n_points[i];
        centroids[i].y = sum_y[i] / n_points[i];
    }
}

int has_converged() {
    for (int i = 0; i < K; i++)
        if (centroids[i].x != old_centroids[i].x || centroids[i].y != old_centroids[i].y)
            return 0;
    return 1;
}

void copy_centroids() {
    for (int i = 0; i < K; i++) {
        old_centroids[i].x = centroids[i].x;
        old_centroids[i].y = centroids[i].y;
    }
}

int main() {
    int iterations = -1;

    aloca();

    inicializa();

    while (iterations == -1 || !has_converged()) {
        copy_centroids();
        cluster_points();
        reevaluate_points();
        iterations++;
    }

    printf("N = %d, K = %d\n", N, K);
    for (int i = 0; i < K; i++)
        printf("Center: (%.3f, %.3f) : Size: %d\n", centroids[i].x, centroids[i].y, n_points[i]);
    printf("Iterations: %d\n", iterations);

    free(points);
    free(centroids);
    free(old_centroids);

    return 0;
}