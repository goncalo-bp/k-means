#include <stdio.h>
#include <stdlib.h>

#define N 10000000
#define K 4

typedef struct point {
    float x;
    float y;
    int cluster;
    float minDist;
} Point;

typedef struct cluster {
    float x;
    float y;
    int nPoints;
} Cluster;

Point *new_point(float x, float y) {
    Point *p = malloc(sizeof(struct point));
    p->x = x;
    p->y = y;
    p->cluster = -1;
    p->minDist = TMP_MAX;
    return p;
}

Cluster *new_cluster(float x, float y) {
    Cluster *c = malloc(sizeof(struct cluster));
    c->x = x;
    c->y = y;
    c->nPoints = 0;
    return c;
}

float distance(float x1, float y1, float x2, float y2) {
    return (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2);
}

void inicializa(Point **points, Cluster **clusters) {
    srand(10);
    for(int i = 0; i < N; i++) {
        float x = (float) rand() / RAND_MAX;
        float y = (float) rand() / RAND_MAX;
        points[i] = new_point(x, y);
    }
    for(int i = 0; i < K; i++) {
        float x = points[i]->x;
        float y = points[i]->y;
        clusters[i] = new_cluster(x, y);
    }
}

void set_distance(Point **points, Cluster **clusters) { // OTIMIZAR (Ciclo dentro do outro)
    for (int j = 0; j < N; j++) {
        float dist = distance(points[j]->x, points[j]->y , clusters[0]->x, clusters[0]->y);
        points[j]->minDist = dist;
        points[j]->cluster = 0;
        for (int i = 1; i < K; i++) {
            dist = distance(points[j]->x, points[j]->y , clusters[i]->x, clusters[i]->y);
            if (dist < points[j]->minDist) {
                points[j]->cluster = i;
                points[j]->minDist = dist;
            }
        }
        clusters[points[j]->cluster]->nPoints += 1;
    }
}

void semnome(Point **points, Cluster **clusters) {
    float sumX[K], sumY[K];
    for (int i = 0; i < K; i++) {
        sumX[i] = 0;
        sumY[i] = 0;
    }

    for (int i = 0; i < N; i++) {
        sumX[points[i]->cluster] += points[i]->x;
        sumY[points[i]->cluster] += points[i]->y;

        points[i]->minDist = TMP_MAX;
    }

    for (int i = 0; i < K; i++) {
        clusters[i]->x = sumX[i] / clusters[i]->nPoints;
        clusters[i]->y = sumY[i] / clusters[i]->nPoints;
    }
}

int has_converged(Cluster **clusters, Cluster **oldClusters) {
    for (int i = 0; i < K; i++)
        if (clusters[i]->x != oldClusters[i]->x || clusters[i]->y != oldClusters[i]->y)
            return 0;
    return 1;
}

void copy_clusters(Cluster **clusters, Cluster **oldClusters) {
    for (int i = 0; i < K; i++) {
        oldClusters[i] = new_cluster(clusters[i]->x, clusters[i]->y);
        clusters[i]->nPoints = 0;
    }
}

int main() {
    int iterations = -1;

    Point **points = malloc(sizeof(Point *) * N);
    Cluster **clusters = malloc(sizeof(Cluster *) * K);
    Cluster **oldClusters = malloc(sizeof(Cluster *) * K);

    inicializa(points, clusters);

    while (iterations == -1 || !has_converged(clusters, oldClusters)) {
        copy_clusters(clusters, oldClusters);
        set_distance(points, clusters);
        semnome(points, clusters);
        iterations++;
    }

    printf("N = %d, K = %d\n", N, K);
    for (int i = 0; i < K; i++)
        printf("Center: (%.3f, %.3f) : Size: %d\n", clusters[i]->x, clusters[i]->y, clusters[i]->nPoints);
    printf("Iterations: %d\n", iterations);

    free(points);
    free(clusters);
    free(oldClusters);

    return 0;
}