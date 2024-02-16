#include "execcon.h"

int main(int argc, char * argv[]) {
    if (argc == 1) return -1;
    for (int i = 1; i < argc; i++) {
        if (fork() == 0) {
            /* Código processos-filho */
            execlp(argv[i], argv[i], NULL);
            _exit(8);
        }
    }
    /* Código processo-pai */
    for (int j = 1; j < argc; j++) {
        int status;
        int terminated_process = wait(&status);
        printf("exit status: %d\n", WEXITSTATUS(status));
    }
    return 0;
}
