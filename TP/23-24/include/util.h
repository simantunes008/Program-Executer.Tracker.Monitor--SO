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
#define MAXARGS  20
#define MAXPIPES 20

typedef struct task {
    pid_t pid;
    int time;
    char cmd[20];
    char prog[MAXBYTES];
} Task;

typedef struct entry {
    pid_t pid;
    long int texec;
    char prog[MAXBYTES];
} Entry;

int execute_u(char* prog, char* file) {
    char **array = malloc(MAXARGS * sizeof(char*));
    char *prog_copy = strdup(prog);
    char *temp;
    int i = 0;

    while ((temp = strsep(&prog_copy, " ")) != NULL) {
        array[i++] = temp;
    }

    array[i] = NULL;
    free(prog_copy);

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

    if (fork() == 0) {
        execvp(array[0], array);
        _exit(1);
    }

    close(fd);

    dup2(stdout_backup, STDOUT_FILENO);
    dup2(stderr_backup, STDERR_FILENO);
    close(stdout_backup);
    close(stderr_backup);

    wait(&res);
    if (WIFEXITED(res)) {
        return -1;
    }

    return 0;
}

char **parsePipes(char *prog) {
    char **array = malloc(MAXPIPES * sizeof(char*));
    char *prog_copy = strdup(prog);
    char *temp;
    int i = 0;

    while ((temp = strsep(&prog_copy, "|")) != NULL) {
        if (i != 0) {
            array[i++] = temp + 1; // ! Para não copiar os espaços
        } else {
            array[i++] = temp;
        }
    }

    array[i] = NULL;
    free(prog_copy);

    return array;
}

char ***parseArgs(char **prog){
    char ***matrix = malloc(MAXPIPES * sizeof(char **));
    int i = 0;
    
    while (prog[i] != NULL) {
        char *prog_copy = strdup(prog[i]);
        char *temp;
        int j = 0;
        
        matrix[i] = malloc(MAXARGS * sizeof(char *));
        
        while ((temp = strsep(&prog_copy, " ")) != NULL) {
            if (strcmp(temp, "") != 0) {
				matrix[i][j++] = temp;
			}
        }

        matrix[i][j] = NULL;
        free(prog_copy);
        i++;
    }
    
    matrix[i] = NULL;

    return matrix;
}

int execute_p(char *prog, char* file) {
    char **array = parsePipes(prog);
    char ***matrix = parseArgs(array);

    int len = 0;
    while (matrix[len] != NULL) {
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
                execvp(matrix[i][0], matrix[i]);
                _exit(1);
            } else {
                close(pipes[i][1]);
                wait(&res);
                if (WEXITSTATUS(res)) {
                    return -1;
                }
            }
        } else if (i == len - 1) {
            if (fork() == 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
                close(pipes[i - 1][0]);
                execvp(matrix[i][0], matrix[i]);
                _exit(1);
            } else {
                close(pipes[i - 1][0]);
                wait(&res);
                if (WEXITSTATUS(res)) {
                    return -1;
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
                execvp(matrix[i][0], matrix[i]);
                _exit(1);
            } else {
                close(pipes[i - 1][0]);
                close(pipes[i][1]);
                wait(&res);
                if (WEXITSTATUS(res)) {
                    return -1;
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