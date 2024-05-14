#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h> /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */

/* lsof -p PID */

int main() {
    int pipe_fd[2];
    if (pipe(pipe_fd) < 0) {
        perror("pipe");
        return -1;
    }
    pid_t pid;
    if ((pid = fork()) == 0) {
        close(pipe_fd[0]);
        int value = 69;
        for (int n = 0; n < 20000; n++) {
            write(pipe_fd[1], &value, sizeof(int));
            printf("write nº %d\n", n);
        }
        _exit(0);
    }
    close(pipe_fd[1]);
    int status, value;
    sleep(5);
    for (int n = 0; n < 20000; n++) {
        read(pipe_fd[0], &value, sizeof(int));
    }
    printf("read returned...\n");
    printf("value: %d\n", value);
    if (wait(&status) > 0 && WIFEXITED(status)) {
        printf("exit status: %d\n", WEXITSTATUS(status));
    }
    return 0;
}
