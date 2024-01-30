#include "pid.h"

int main(int argc, char * argv[]) {
    printf("pid: %d\n", getpid());
    printf("ppid: %d\n", getppid());
    // sleep(10);
    return 0;
}
