#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

typedef struct point {
    float x;
    float y;
} Point;

int N;
int K;

Point *points __attribute__((aligned (32)));
Point *centroids __attribute__((aligned (32)));
int *n_points __attribute__((aligned (32)));
float *new_x __attribute__((aligned (32)));
float *new_y __attribute__((aligned (32)));
int has_converged;

// Aloca espaço para as variáveis globais
void aloca() {
    points = malloc(sizeof(struct point) * N);
    centroids = malloc(sizeof(struct point) * K);
    n_points = malloc(sizeof(int) * K);
    new_x = malloc(sizeof(float) * K);
    new_y = malloc(sizeof(float) * K);
    has_converged = 0;
}

// Inicializa os pontos com valores aleatóris e os centroides com os valores dos primeiros K pontos
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

// Calcula a distância entre dois pontos
float distance(Point p1, Point p2) {
    return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}

// Calcula as distâncias dos pontos aos clusters, associa-os ao cluster mais próximo e reavalia as coordenadas dos clusters
void cluster_points() { 
    #pragma omp parallel for
    for (int i = 0; i < K; i++) {
        new_x[i] = 0; 
        new_y[i] = 0; 
        n_points[i] = 0; 
    }

    // Cálculo das distancias dos pontos aos clusters e associação ao cluster mais próximo
    #pragma omp parallel
    {
        #pragma omp for schedule(static) reduction(+ : new_x[:K], new_y[:K], n_points[:K])
        for (int j = 0; j < N; j++) {
            float dist = distance(points[j], centroids[0]);
            int cluster_id = 0;
            for (int i = 1; i < K; i++) {        
                float tmp = distance(points[j], centroids[i]);
                if (tmp < dist) {
                    cluster_id = i;
                    dist = tmp;
                }
            }

            n_points[cluster_id]++;
            new_x[cluster_id] += points[j].x;
            new_y[cluster_id] += points[j].y;
        }
    }
    
    /*
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < N; i++) {
        float dists[K];
        #pragma omp parallel for schedule(static)
        for (int j = 0; j < K; j++) {
            dists[j] = distance(points[i], centroids[j]);
        }

        float min_dist = dists[0];
        int cluster = 0;
        for (int j = 1; j < K; j++) {
            if (dists[j] < min_dist) {
                cluster = j;
                float min_dist = dists[j];
            }
        }
        
        #pragma omp atomic
        new_x[cluster] += points[i].x;
        #pragma omp atomic
        new_y[cluster] += points[i].y;
        #pragma omp atomic
        n_points[cluster]++;
    }
    */
   #pragma omp parallel for
        for (int i = 0; i < K; i++) {
            centroids[i].x = new_x[i] / n_points[i];;
            centroids[i].y = new_y[i] / n_points[i];;
        }
}

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Número argumentos: %d\n", argc);
        printf("Argumentos insuficientes: ./k_means <N> <K>\n");
        return -1;
    }
    N = atoi(argv[1]);
    K = atoi(argv[2]);
    if (argc == 4)
        omp_set_num_threads(atoi(argv[3]));
    
    int iterations = -1;

    aloca();

    inicializa();

    while (iterations < 20) {
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

/*
qg2PbeK5
*/