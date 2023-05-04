//
// Created by simao on 14-04-2023.
//

#include "../includes/includes.h"

int fd_DEFAULT, fd_STATUS, fd_TERMINATED, fd_CS, fd_SC, fd_pipe[2], running_programs = 0;
Client_info running[MAX_PROCESSES];

void handler(int signal) {
    if (signal == SIGINT) { /* Fecha tudo em segurança quando recebe o sinal SIGINT */
        close(fd_DEFAULT);
        close(fd_STATUS);
        close(fd_CS);
        close(fd_SC);
        unlink(FIFO_DEFAULT);
        unlink(FIFO_STATUS);
        write(STDOUT_FILENO, "\nClosing server...\n", 20);
        exit(0);
    }
}

void status() {
    long TEXEC;
    char status_buf[256];
    struct timeval now;
    gettimeofday(&now, NULL);
    ssize_t read_bytes;
    fd_STATUS = open(FIFO_STATUS, O_WRONLY);
    for (int i = 0; i < running_programs; i++) {
        TEXEC = (now.tv_sec - running[i].start.tv_sec) * 1000 + (now.tv_usec - running[i].start.tv_usec) / 1000;
        read_bytes = sprintf(status_buf, "%d %s %ld ms\n", running[i].pid, running[i].name, TEXEC);
        write(fd_STATUS, status_buf, read_bytes);
    }
    close(fd_STATUS);
}

int in_running(pid_t pid) {
    for (int i = 0; i < running_programs; i++) {
        if (running[i].pid == pid) {
            return 1;
        }
    }
    return 0;
}

void remove_client(pid_t pid) {
    for (int i = 0; i < running_programs; i++) {
        if (running[i].pid == pid) {
            for (int j = i; j < running_programs - 1; j++) {
                running[j] = running[j + 1];
            }
            running_programs--;
            break;
        }
    }
}

int main() {
    char * endptr, buf[20];
    signal(SIGINT, handler); // SIGINT é o sinal enviado quando se faz CTRL + C

    /* Cria os pipes definidos no ficheiro includes.h */
    mkfifo(FIFO_DEFAULT, 0622); // R&W--W--W
    mkfifo(FIFO_STATUS, 0622);

    while (1) {
        if ((fd_DEFAULT = open(FIFO_DEFAULT, O_RDONLY)) == -1) {
            perror("open");
            exit(1);
        }

        /* Cria um pipe anónimo */
        if (pipe(fd_pipe) < 0) {
            perror("pipe");
            exit(1);
        }

        while((read(fd_DEFAULT, buf, 20)) > 0) {
            if (!strcmp(buf, "status")) {
                if (fork() == 0) {
                    status();
                    exit(0);
                }
            }
            else {
                if (in_running((pid_t) strtol(buf, &endptr, 10))) {
                    remove_client((pid_t) strtol(buf, &endptr, 10));
                }
                else {
                    if (fork() == 0) {
                        Client_info client;
                        char pid_buf[20], CS_path[256], SC_path[256];

                        /* Fecha o descritor de leitura */
                        close(fd_pipe[0]);

                        /* Formata as strings com o caminho para os pipes do cliente */
                        sprintf(CS_path, "../tmp/CS%s", buf);
                        sprintf(SC_path, "../tmp/SC%s", buf);

                        /* Abre o pipe para ler a informação do cliente */
                        fd_CS = open(CS_path, O_RDONLY);
                        while (read(fd_CS, &client, 56) > 0) {
                            write(fd_pipe[1], &client, 56);
                        }
                        close(fd_CS);

                        /* Quando acaba de executar reenvia o pid */
                        sprintf(pid_buf, "%d", client.pid);
                        fd_TERMINATED = open(FIFO_DEFAULT, O_WRONLY);
                        write(fd_TERMINATED, pid_buf, 20);
                        close(fd_TERMINATED);

                        /* Calcula o tempo de execução em milissegundos */
                        long TEXEC = (client.end.tv_sec - client.start.tv_sec) * 1000 + (client.end.tv_usec - client.start.tv_usec) / 1000;

                        /* Abre o pipe para escrever o tempo de execução */
                        fd_SC = open(SC_path, O_WRONLY);
                        write(fd_SC, &TEXEC, 8);
                        close(fd_SC);
                        exit(0);
                    }
                    if (running_programs < MAX_PROCESSES) {
                        Client_info client;
                        close(fd_pipe[1]);
                        read(fd_pipe[0], &client, 56);
                        running[running_programs++] = client;
                    }
                }
            }
        }
        close(fd_DEFAULT);
    }
}