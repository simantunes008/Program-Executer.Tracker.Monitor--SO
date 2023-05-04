//
// Created by simao on 14-04-2023.
//

#include "../includes/includes.h"

int fd_DEFAULT, fd_STATUS, fd_CS, fd_SC;

int execute(char * argv[]) {
    int status;
    if (fork() == 0) {
        execvp(argv[0], argv);
        exit(1);
    }
    wait(&status);
    if (WIFEXITED(status)) status = WEXITSTATUS(status);
    return status;
}

int main(int argc, char * argv[]) {
    if (argc == 1) { /* Caso não sejam passados argumentos imprime as opções de utilização */
        write(STDOUT_FILENO, "./tracer execute -u \"prog-a arg-1 (...) arg-n\"\n./tracer execute -p \"prog-a arg-1 (...) arg-n | prog-b arg-1 (...) arg-n | prog-c arg-1 (...) arg-n\"\n./tracer status\n", 165);
    }
    else if (argc == 2 && !strcmp(argv[1], "status")) {
        char output_buf[20];
        ssize_t read_bytes;

        /* Abre o pipe DEFAULT e envia a mensagem "status" para o servidor */
        if ((fd_DEFAULT = open(FIFO_DEFAULT, O_WRONLY)) == -1) {
            perror("open");
            exit(1);
        }
        write(fd_DEFAULT, "status", 7);
        close(fd_DEFAULT);

        /* Recebe e imprime a lista de programas em execução do servidor */
        fd_STATUS = open(FIFO_STATUS, O_RDONLY);
        while ((read_bytes = read(fd_STATUS, output_buf, 20)) > 0) {
            write(STDOUT_FILENO, output_buf, read_bytes);
        }
        close(fd_STATUS);
    }
    else if (argc == 4 && !strcmp(argv[1], "execute") && !strcmp(argv[2], "-u")) {
        long TEXEC;
        int output_bytes, i = 0;
        char * token, * argvs[20], pid_buf[20], output_buf[20], CS_path[256], SC_path[256];
        Client_info client;

        /* Divide o comando em várias "strings" */
        token = strtok(argv[3], " ");
        while (token != NULL && i < 20) {
            argvs[i++] = token;
            token = strtok(NULL, " ");
        }
        argvs[i] = NULL;

        /* Obtém o nome do comando e o pid do cliente */
        strncpy(client.name, argvs[0], 20);
        client.pid = getpid();

        /* Abre o pipe DEFAULT, imprime o pid e envia uma mensagem com o mesmo para o servidor */
        if ((fd_DEFAULT = open(FIFO_DEFAULT,O_WRONLY)) == -1) {
            perror("open");
            exit(1);
        }
        sprintf(pid_buf, "%d", client.pid);
        output_bytes = snprintf(output_buf, 20, "Running PID %d\n", client.pid);
        write(fd_DEFAULT, pid_buf, 20);
        write(STDOUT_FILENO, output_buf, output_bytes);
        close(fd_DEFAULT);
        memset(output_buf, 0, 20); // Limpa a "string" para ser reutilizada

        /* Cria os pipes de comunicação com o servidor */
        snprintf(CS_path, 256,"../tmp/CS%d", client.pid);
        snprintf(SC_path, 256,"../tmp/SC%d", client.pid);
        mkfifo(CS_path, 0622);
        mkfifo(SC_path, 0622);

        /* Envia a informação do cliente antes da execução do programa, executa-o e envia a informação do cliente após a execução*/
        fd_CS = open(CS_path, O_WRONLY);
        gettimeofday(&client.start, NULL);
        write(fd_CS, &client, 56);
        execute(argvs); // Executa o programa
        gettimeofday(&client.end, NULL);
        write(fd_CS, &client, 56);
        close(fd_CS);

        /* Imprime o tempo de execução retornado pelo servidor */
        fd_SC = open(SC_path, O_RDONLY);
        read(fd_SC, &TEXEC, 8);
        close(fd_SC);
        output_bytes = snprintf(output_buf, 20, "Ended in %ld ms\n", TEXEC);
        write(STDOUT_FILENO, output_buf, output_bytes);

        /* Fecha os pipes de comunicação com o servidor */
        unlink(CS_path);
        unlink(SC_path);
    }
    else if (argc == 4 && !strcmp(argv[1], "execute") && !strcmp(argv[2], "-p")) {
        long TEXEC;
        int output_bytes, i = 0;
        char * token, * command, * argvs[20][20], pid_buf[20], output_buf[20], CS_path[256], SC_path[256], name[20] = {};
        Client_info client;

        /* Divide os comandos numa matriz de "strings" */
        while ((command = strsep(&argv[3], "|")) != NULL && i < 5) {
            int j = 0;
            token = strtok(command, " ");
            while (token != NULL && j < 20) {
                argvs[i][j++] = token;
                token = strtok(NULL, " ");
            }
            argvs[i++][j] = NULL;
        }
        argvs[i][0] = NULL;

        /* Transforma o nome dos comandos numa só “string” para enviar para ao servidor */
        strcat(name, argvs[0][0]);
        for (int k = 1; k < i; k++) {
            strcat(name, " | ");
            strcat(name, argvs[k][0]);
        }
        strncpy(client.name, name, 20);
        client.pid = getpid();

        /* Abre o pipe DEFAULT, imprime o pid e envia uma mensagem com o mesmo para o servidor */
        if ((fd_DEFAULT = open(FIFO_DEFAULT,O_WRONLY)) == -1) {
            perror("open");
            exit(1);
        }
        sprintf(pid_buf, "%d", client.pid);
        output_bytes = snprintf(output_buf, 20, "Running PID %d\n", client.pid);
        write(fd_DEFAULT, pid_buf, 20);
        write(STDOUT_FILENO, output_buf, output_bytes);
        close(fd_DEFAULT);
        memset(output_buf, 0, 20);

        /* Cria os pipes de comunicação com o servidor */
        snprintf(CS_path, 256,"../tmp/CS%d", client.pid);
        snprintf(SC_path, 256,"../tmp/SC%d", client.pid);
        mkfifo(CS_path, 0622);
        mkfifo(SC_path, 0622);

        /* Envia a informação do cliente antes da execução do pipeline, executa-o e envia a informação do cliente após a execução*/
        fd_CS = open(CS_path, O_WRONLY);
        gettimeofday(&client.start, NULL);
        write(fd_CS, &client, 56);
        int fd_pipe[i - 1][2]; // Descritores entre processos-filho
        pid_t pids[i];
        for (int k = 0; k < i; k++) {
            pipe(fd_pipe[k]);
            if ((pids[k] = fork()) == 0) {
                if (k > 0) dup2(fd_pipe[k - 1][0], STDIN_FILENO);
                if (k < i - 1) dup2(fd_pipe[k][1], STDOUT_FILENO);
                for (int j = 0; j < i - 1; j++) {
                    close(fd_pipe[j][0]);
                    close(fd_pipe[j][1]);
                }
                execvp(argvs[k][0], argvs[k]);
            }
            if (k > 0) {
                close(fd_pipe[k - 1][0]);
                close(fd_pipe[k - 1][1]);
            }
        }
        for (int k = 0; k < i; k++) {
            waitpid(pids[k], NULL, 0);
        }
        gettimeofday(&client.end, NULL);
        write(fd_CS, &client, 56);
        close(fd_CS);

        /* Imprime o tempo de execução retornado pelo servidor */
        fd_SC = open(SC_path, O_RDONLY);
        read(fd_SC, &TEXEC, 8);
        close(fd_SC);
        output_bytes = snprintf(output_buf, 20, "Ended in %ld ms\n", TEXEC);
        write(STDOUT_FILENO, output_buf, output_bytes);

        /* Fecha os pipes de comunicação com o servidor */
        unlink(CS_path);
        unlink(SC_path);
    }
    else {
        write(STDOUT_FILENO, "Invalid input\n", 15);
    }
    return 0;
}
