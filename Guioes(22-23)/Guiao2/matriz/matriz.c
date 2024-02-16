#include "matriz.h"

int main(int argc, char * argv[]) {
    if (argc == 1) return -1;
    pid_t pid;
    int status, i, j, a = atoi(argv[1]);
    srand(time(NULL)); /* Inicializa o randomizer */
    int matriz[M][N];
    for(i = 0; i < M; i++) {
        for (j = 0; j < N; j++) {
            matriz[i][j] = rand() % 64; /* Gera valores aleatórios entre 0 e 63 */
        }
    }
    for (i = 0; i < M; i++) {
        if ((pid = fork()) == 0) {
            /* Código processos-filho */
            for (j = 0; j < N; j++) {
                if (matriz[i][j] == a) {
                    printf("[filho] pid: %d | ppid: %d | found it\n", getpid(), getppid());
                    _exit(1); /* Elemento encontrado */
                }
            }
            _exit(0);
        }
    }
    for (int k = 0; k < M; k++) {
        pid_t terminated_pid = wait(&status);
        if (WIFEXITED(status)) {
            printf("[pai] pid: %d | terminated process: %d | exit value: %d\n", getpid(), terminated_pid, WEXITSTATUS(status));
        }
        else {
            printf("bad exit\n");
        }
    }
    return 0;
}
