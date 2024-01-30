#include "mysystem.h"

int mysystemV1(const char * command) {
    char * args[24];
    int j = 0, index = 0, building = 0, status;
    for(int i = 0; command[i] != '\0'; i++) {
        if (command[i] == ' ' && building == 1) {
            args[index++][j] = '\0';
            building = j = 0;
        }
        if (command[i] != ' ' && building == 1) {
            args[index][j++] = command[i];
        }
        if (command[i] != ' ' && building == 0) {
            args[index] = malloc(sizeof(char *));
            args[index][j++] = command[i];
            building = 1;
        }
    }
    if (building == 1) {
        args[index++][j] = '\0';
    }
    args[index] = NULL;
    if (fork() == 0) {
        execvp(args[0], args);
        _exit(-1);
    }
    wait(&status);
    if (WIFEXITED(status)) status = WEXITSTATUS(status);
    return status;
}

int mysystemV2(const char * command) {
    int status;
    if (fork() == 0) {
        const char * args[] = {"/bin/sh", "-c", command, NULL };
        /* O primeiro argumento é o caminho para a shell, o segundo (-c)
         * indica que o próximo argumento é um comando a ser executado e
         * o terceiro é o comando que queremos executar */
        execv(args[0], (char * const *)args);
        _exit(-1);
    }
    wait(&status);
    if (WIFEXITED(status)) status = WEXITSTATUS(status);
    return status;
}

int main(int argc, char * argv[]) {
    mysystemV2("sopa"); /* Testar um comando não válido */
    system("sopa");
    mysystemV2("ls -l"); /* Testar um comando com dois argumentos */
    system("ls -l");
    mysystemV2("cat ex.txt"); /* Testar um comando com um arquivo de texto */
    system("cat ex.txt");
    return 0;
}
