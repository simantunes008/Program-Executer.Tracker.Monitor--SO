#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#define MAXBYTES 300
#define MAX_ARGS 4

typedef struct task {
    pid_t pid;
    int time;
    char cmd[MAXBYTES];
    char prog[MAXBYTES];
} Task;

typedef struct entry {
    pid_t pid;
    long int texec;
    char prog[MAXBYTES];
} Entry;

int mysystem(const char* command, char* file) {
    int res = 0, arg_count = 0;
    char *arg_values[MAX_ARGS];

    char command_copy[MAXBYTES];
    strcpy(command_copy, command);

    int fd = open(file, O_WRONLY | O_CREAT, 0777);
    if (fd == -1) {
        perror("Failed to create output file");
        return -1;
    }

    int stdout_backup = dup(STDOUT_FILENO);
    int stderr_backup = dup(STDERR_FILENO);

    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);

    char *token = strtok(command_copy, " ");
    while (token != NULL && arg_count < MAX_ARGS - 1) {
        arg_values[arg_count++] = token;
        token = strtok(NULL, " ");
    }
    arg_values[arg_count] = NULL;

    if (fork() == 0) {
        execvp(arg_values[0], arg_values);
        _exit(1);
    }

    close(fd);

    wait(&res);
    if (WIFEXITED(res)) res = -WEXITSTATUS(res);

    dup2(stdout_backup, STDOUT_FILENO);
    dup2(stderr_backup, STDERR_FILENO);
    close(stdout_backup);
    close(stderr_backup);

    return res;
}