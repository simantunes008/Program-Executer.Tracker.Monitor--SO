//
// Created by simao on 27-04-2023.
//

#ifndef INC_034_INCLUDES_H
#define INC_034_INCLUDES_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/stat.h>
#define FIFO_DEFAULT "../tmp/DEFAULT" /* Pipe geral entre todos os clientes e o servidor */
#define FIFO_STATUS "../tmp/STATUS" /* Pipe para enviar a lista do programas abertos */
#define MAX_PROCESSES 1000 /* Define o número máximo de processos que podem estar em execução simultaneamente */

typedef struct Client_info { /* Struct com as informações do Cliente a enviar para o Servidor */
    pid_t pid;
    char name[20];
    struct timeval start, end;
} Client_info;

#endif //INC_034_INCLUDES_H
