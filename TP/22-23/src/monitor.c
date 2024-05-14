#include "../includes/includes.h"

int fd_DEFAULT, fd_STATUS, fd_TERMINATED, fd_CS, fd_SC, fd_LOGS, fd_pipe[2], running_programs = 0;
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
    char status_buf[BUFFER_SIZE];
    struct timeval now;
    gettimeofday(&now, NULL);
    ssize_t read_bytes;
    fd_STATUS = open(FIFO_STATUS, O_WRONLY);
    for (int i = 0; i < running_programs; i++) {
        TEXEC = (now.tv_sec - running[i].start.tv_sec) * 1000 + (now.tv_usec - running[i].start.tv_usec) / 1000;
        read_bytes = snprintf(status_buf, BUFFER_SIZE, "%d %s %ld ms\n", running[i].pid, running[i].name, TEXEC);
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

int main(__attribute__((unused)) int argc, char * argv[]) {
    char * endptr, buf[BUFFER_SIZE];
    signal(SIGINT, handler); // SIGINT é o sinal enviado quando se faz CTRL + C

    /* Cria os pipes definidos no ficheiro includes.h */
    if (mkfifo(FIFO_DEFAULT, 0622) == -1) { // R&W--W--W
        perror("mkfifo");
        exit(1);
    }
    if (mkfifo(FIFO_STATUS, 0644) == -1) { // R&W--R--R
        perror("mkfifo");
        exit(1);
    }

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

        while((read(fd_DEFAULT, buf, BUFFER_SIZE)) > 0) {
            if (!strcmp(buf, "status")) {
                status();
            }
            else {
                if (in_running((pid_t) strtol(buf, &endptr, 10))) {
                    remove_client((pid_t) strtol(buf, &endptr, 10));
                }
                else {
                    if (fork() == 0) {
                        Client_info client;
                        char pid_buf[BUFFER_SIZE], CS_path[PATH_SIZE], SC_path[PATH_SIZE], logs_path[PATH_SIZE];

                        /* Fecha o descritor de leitura */
                        close(fd_pipe[0]);

                        /* Formata as strings com o caminho para os pipes do cliente */
                        snprintf(CS_path, PATH_SIZE, "../tmp/CS%s", buf);
                        snprintf(SC_path, PATH_SIZE, "../tmp/SC%s", buf);

                        /* Abre o pipe para ler a informação do cliente */
                        if ((fd_CS = open(CS_path, O_RDONLY)) == -1) {
                            perror("open");
                            exit(1);
                        }
                        while (read(fd_CS, &client, sizeof(client)) > 0) {
                            write(fd_pipe[1], &client, sizeof(client));
                        }
                        close(fd_CS);

                        /* Quando acaba de executar reenvia o pid */
                        snprintf(pid_buf, BUFFER_SIZE, "%d", client.pid);
                        if ((fd_TERMINATED = open(FIFO_DEFAULT, O_WRONLY)) == -1) {
                            perror("open");
                            exit(1);
                        }
                        write(fd_TERMINATED, pid_buf, BUFFER_SIZE);
                        close(fd_TERMINATED);

                        /* Calcula o tempo de execução em milissegundos */
                        long TEXEC = (client.end.tv_sec - client.start.tv_sec) * 1000 + (client.end.tv_usec - client.start.tv_usec) / 1000;

                        /* Abre o pipe para escrever o tempo de execução */
                        if ((fd_SC = open(SC_path, O_WRONLY)) == -1) {
                            perror("open");
                            exit(1);
                        }
                        write(fd_SC, &TEXEC, 8);
                        close(fd_SC);

                        /* Cria um ficheiro binário para armazenar a informação do cliente */
                        snprintf(logs_path, PATH_SIZE, "%s/%s.bin", argv[1], buf);
                        fd_LOGS = open(logs_path, O_CREAT | O_WRONLY | O_APPEND, 0622);
                        write(fd_LOGS, &client.name, 200);
                        write(fd_LOGS, &TEXEC, 8);
                        close(fd_LOGS);
                        exit(0);
                    }
                    if (running_programs < MAX_PROCESSES) {
                        Client_info client;
                        close(fd_pipe[1]);
                        read(fd_pipe[0], &client, sizeof(client));
                        running[running_programs++] = client;
                    }
                }
            }
        }
        close(fd_DEFAULT);
    }
}
