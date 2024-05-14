#include <stdio.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <sys/wait.h> /* chamadas wait*() e macros relacionadas */

int main() {
    pid_t pid;
    int status;
    for (int i = 1; i <= 10; i++) {
        if ((pid = fork()) == 0) {
            /* Código processos-filho */
            printf("[filho] pid: %d | ppid: %d\n", getpid(), getppid());
            _exit(i); /* Para os processos-filho não criarem outros processos */
        }
    }
    for (int j = 1; j <= 10; j++) {
        pid_t terminated_pid = wait(&status);
        if (WIFEXITED(status)) {
            printf("[pai] terminated process: %d | exit value: %d\n", terminated_pid, WEXITSTATUS(status));
        }
        else {
            printf("bad exit\n");
        }
    }
    return 0;
}
