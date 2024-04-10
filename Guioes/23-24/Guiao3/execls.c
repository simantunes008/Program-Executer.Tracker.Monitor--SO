#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char ** argv) {
    char * args[3] = {"ls", "-l", NULL};
    if (fork() == 0) {
        execvp("ls", args);
        _exit(0);
    }
    wait(NULL);
    return 0;
}
