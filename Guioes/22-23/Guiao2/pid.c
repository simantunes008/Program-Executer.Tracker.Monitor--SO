#include <stdio.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <sys/wait.h> /* chamadas wait*() e macros relacionadas */

int main(int argc, char * argv[]) {
    printf("pid: %d\n", getpid());
    printf("ppid: %d\n", getppid());
    // sleep(10);
    return 0;
}
