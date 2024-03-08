#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

int main(int argc, char ** argv) {
    int status;
    for (int i = 0; i < 10; i++) {
        if (fork() == 0) {
            printf("[child] pid: %d | ppid: %d\n", getpid(), getppid());
            _exit(i);
        }
        wait(&status);
        if (WIFEXITED(status)) {
            printf("[father] exit value: %d\n", WEXITSTATUS(status));
        } else {
            printf("[father] bad exit\n");
        }
    }
    return 0;
}
