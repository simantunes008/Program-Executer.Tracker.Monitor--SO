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
    char **array = malloc(20 * sizeof(char*));
    char *command_copy = strdup(command);
    char *aux;
    int i = 0;

    while ((aux = strsep(&command_copy, " ")) != NULL) {
        array[i++] = aux;
    }

    array[i] = NULL;
    free(command_copy);

    int res = 0;

    int fd = open(file, O_WRONLY | O_CREAT, 0777);
    if (fd == -1) {
        perror("Failed to create output file");
        return -1;
    }

    int stdout_backup = dup(STDOUT_FILENO);
    int stderr_backup = dup(STDERR_FILENO);

    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);

    if (fork() == 0) {
        execvp(array[0], array);
        perror("Failed to Execute Command!");
        _exit(1);
    }

    close(fd);

    dup2(stdout_backup, STDOUT_FILENO);
    dup2(stderr_backup, STDERR_FILENO);
    close(stdout_backup);
    close(stderr_backup);

    wait(&res);
    if (WIFEXITED(res)) {
        return 1;
    }

    return 0;
}

char **parsePipes(char *prog) {
    char **array = malloc(20 * sizeof(char*));
    char *prog_copy = strdup(prog);
    char *aux;
    int i = 0;

    while ((aux = strsep(&prog_copy, "|")) != NULL) {
        if (i != 0) {
            array[i++] = aux + 1; // ! Para não copiar os espaços
        } else {
            array[i++] = aux;
        }
    }

    array[i] = NULL;
    free(prog_copy);
    return array;
}

char ***parseArgs(char **prog){
    char ***matriz = malloc(20 * sizeof(char **));
    int i = 0;
    
    while (prog[i] != NULL) {
        char *prog_copy = strdup(prog[i]);
        char *aux;
        int j = 0;
        
        matriz[i] = malloc(20 * sizeof(char *));
        
        while ((aux = strsep(&prog_copy, " ")) != NULL) {
            if (strcmp(aux, "") != 0) {
				matriz[i][j++] = aux;
			}
        }

        matriz[i][j] = NULL;
        free(prog_copy);
        i++;
    }
    matriz[i] = NULL;
    return matriz;
}

int pipeline(char ***cmd, char* file) {
    int len = 0;
    while (cmd[len] != NULL) {
        len++;
    }

    int pipes[len - 1][2];
    int res;

    int fd = open(file, O_WRONLY | O_CREAT, 0777);
    if (fd == -1) {
        perror("Failed to create output file");
        return -1;
    }

    int stdout_backup = dup(STDOUT_FILENO);
    int stderr_backup = dup(STDERR_FILENO);

    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);

    for (int i = 0; i < len; i++) {
        if (i == 0) {
            pipe(pipes[i]);
            if (fork() == 0) {
                close(pipes[i][0]);
                dup2(pipes[i][1], STDOUT_FILENO);
                close(pipes[i][1]);
                execvp(cmd[i][0], cmd[i]);
                perror("Failed to Execute Command!");
                _exit(1);
            } else {
                close(pipes[i][1]);
                wait(&res);
                if (WEXITSTATUS(res)) {
                    return 1;
                }
            }
        } else if (i == len - 1) {
            if (fork() == 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
                close(pipes[i - 1][0]);
                execvp(cmd[i][0], cmd[i]);
                perror("Failed to Execute Command!");
                _exit(1);
            } else {
                close(pipes[i - 1][0]);
                wait(&res);
                if (WEXITSTATUS(res)) {
                    return 1;
                }
            }
        } else {
            pipe(pipes[i]);
            if (fork() == 0) {
                close(pipes[i][0]);
                dup2(pipes[i - 1][0], STDIN_FILENO);
                close(pipes[i - 1][0]);
                dup2(pipes[i][1], STDOUT_FILENO);
                close(pipes[i][1]);
                execvp(cmd[i][0], cmd[i]);
                perror("Failed to Execute Command!");
                _exit(1);
            } else {
                close(pipes[i - 1][0]);
                close(pipes[i][1]);
                wait(&res);
                if (WEXITSTATUS(res)) {
                    return 1;
                }
            }
        }
    }

    close(fd);

    dup2(stdout_backup, STDOUT_FILENO);
    dup2(stderr_backup, STDERR_FILENO);
    close(stdout_backup);
    close(stderr_backup);

    return 0;
}