#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h> /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */

#define M 4
#define N 8

void printArray(int* array, int lenght) {
    int i = 0;
    while (i < lenght) {
        printf("%d", array[i]);
        i++;
    }
    putchar('\n');
}

void imprimeMatriz(int m, int n, int** matriz) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", matriz[i][j]);
        }
        putchar('\n');
    }
}

int** matrizGenerator() {
    srand(time(NULL));
    int** matriz = malloc(sizeof (int*) * M);
    for (int i = 0; i < M; i++) {
        matriz[i] = malloc (sizeof (int) * N);
        for (int j = 0; j < N; j++) {
            matriz[i][j] = rand() % 16;
        }
    }
    return matriz;
}

int* matrizLookUp(int** matriz, int value, int* results) {
    int pipe_fd[2];
    if (pipe(pipe_fd) < 0) {
        perror("pipe failed");
        return NULL;
    }
    pid_t pid;
    for (int i = 0; i < M; i++) {
        if ((pid = fork()) == 0) {
            close(pipe_fd[0]);
            int count = 0;
            for (int j = 0; j < N; j++) {
                if (matriz[i][j] == value) {
                    count++;
                }
            }
            write(pipe_fd[1], &count, sizeof(int));
            close(pipe_fd[1]);
            _exit(0);
        }
    }
    close(pipe_fd[1]);
    for (int i = 0; i < M; i++) {
        int status, count;
        read(pipe_fd[0], &count, sizeof(int));
        results[i] = count;
        if (wait(&status) > 0 && WIFEXITED(status)) {
            printf("[pai] exit status: %d\n", WEXITSTATUS(status));
        }
    }
    close(pipe_fd[0]);
    return results;
}

int main () {
    int** matriz = matrizGenerator();
    imprimeMatriz(M, N, matriz);
    int results[M];
    matrizLookUp(matriz, 4, results);
    printArray(results, M);
    return 0;
}
