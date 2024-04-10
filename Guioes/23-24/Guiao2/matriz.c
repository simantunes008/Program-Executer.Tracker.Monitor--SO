#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define M 16
#define N 32

void geraMatriz(int m, int n, int ** matriz) {
    srand(time(NULL));
    for(int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            matriz[i][j] = rand() % 64;
        }
    }
}

int main(int argc, char ** argv) {
    int ** matriz = malloc(M * sizeof(int *));
    int status;
    pid_t pid;

    for (int i = 0; i < M; i++) {
        matriz[i] = malloc(N * sizeof(int));
    }

    geraMatriz(M, N, matriz);

    /* Para debug
    for(int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }
    */

    for (int i = 0; i < M; i++) {
        if (fork() == 0) {
            for (int j = 0; j < N; j++) {
                if (matriz[i][j] == atoi(argv[1])) {
                    _exit(i);
                }
            }
            _exit(-1);
        }
        pid = wait(&status);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 255) {
            printf("[father] child process: %d found it in line %d!\n", pid, WEXITSTATUS(status));
        }
    }

    for (int i = 0; i < M; i++) {
        free(matriz[i]);
    }
    free(matriz);

    return 0;
}
