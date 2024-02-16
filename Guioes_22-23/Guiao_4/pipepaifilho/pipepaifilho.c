#include "pipepaifilho.h"

int main() {
    int pipe_fd[2];
    if (pipe(pipe_fd) < 0) { /* Cria um pipe */
        perror("pipe failed");
        return -1;
    }
    pid_t pid;
    if ((pid = fork()) == 0) {
        /* Código processo-filho */
        printf("[filho] waiting\n");
        close(pipe_fd[1]);
        int value;
        read(pipe_fd[0], &value, sizeof(int));
        printf("[filho] valor lido: %d\n", value);
        _exit(0);
    }
    /* Código processo-pai */
    close(pipe_fd[0]);
    int status, value = 69;
    sleep(5);
    write(pipe_fd[1], &value, sizeof(int));
    if (wait(&status) > 0 && WIFEXITED(status)) {
        printf("[pai] exit status: %d\n", WEXITSTATUS(status));
    }
    return 0;
}
