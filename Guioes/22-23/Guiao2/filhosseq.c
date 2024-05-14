#include <stdio.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <sys/wait.h> /* chamadas wait*() e macros relacionadas */

int main() {
    pid_t pid;
    int status, i = 1;
    while (i <= 10) {
        if ((pid = fork()) == 0) {
            /* Código processo-filho */
            printf("child pid: %d\n", getpid());
            printf("child ppid: %d\n", getppid());
            _exit(i);
        }
        /* Código processo-pai */
        if (wait(&status) > 0) {
            if (WIFEXITED(status)) {
                printf("exit value: %d\n", WEXITSTATUS(status));
            }
            else {
                printf("bad exit\n");
            }
        }
        i++;
    }
    return 0;
}
