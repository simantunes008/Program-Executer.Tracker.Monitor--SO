#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <sys/wait.h> /* chamadas wait*() e macros relacionadas */

#define M 4
#define N 8

/* Podem cagar nisto */
void imprimeMatriz(int m, int n, int matriz[m][n]) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", matriz[i][j]);
        }
        putchar('\n');
    }
}

int main(int argc, char * argv[]) {
    if (argc == 1) return -1;
    pid_t pid;
    int status, i, j, a = atoi(argv[1]);
    srand(time(NULL));
    int matriz[M][N];
    for(i = 0; i < M; i++) {
        for (j = 0; j < N; j++) {
            matriz[i][j] = rand() % 16;
        }
    }
    imprimeMatriz(M, N, matriz);
    for (i = 0; i < M; i++) {
        if ((pid = fork()) == 0) {
            /* Código processos-filho */
            for (j = 0; j < N; j++) {
                if (matriz[i][j] == a) {
                    _exit(1);
                }
            }
            _exit(0);
        }
        /* Código processo-pai */
        pid_t terminated_pid = wait(&status);
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == 1) {
                printf("[pai] son process (pid: %d) found it line %d\n", terminated_pid, i + 1);
            }
        }
        else {
            printf("bad exit\n");
        }

    }
    return 0;
}