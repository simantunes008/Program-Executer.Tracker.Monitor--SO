#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct pessoa {
    char nome[200];
    int idade;
} Pessoa;

void adiciona_pessoa(Pessoa * pessoa) {
    int fd = open("pessoas.bin", O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (fd < 0) return;
    write(fd, pessoa, sizeof(Pessoa));
    close(fd);
}

void listar_pessoas(int N) {
    int fd = open("pessoas.bin", O_RDONLY);
    if (fd < 0) return;
    Pessoa * pessoa = malloc(sizeof(Pessoa));
    int bytes_read = 0;
    char buffer[204];
    for (int i = 1; read(fd, pessoa, sizeof(Pessoa)) > 0 && i <= N; i++) {
        bytes_read = snprintf(buffer, 204, "%s %d\n", pessoa -> nome, pessoa -> idade);
        write(STDOUT_FILENO, buffer, bytes_read);
    }
    close(fd);
}

void altera_idade(char * nome, int idade) {
    int fd = open("pessoas.bin", O_RDWR, 0644);
    Pessoa * nova = malloc(sizeof(Pessoa));
    Pessoa * antiga = malloc(sizeof(Pessoa));
    strcpy(nova -> nome, nome);
    nova -> idade = idade;
    while (read(fd, antiga, sizeof(Pessoa)) > 0) {
        if (strcmp(antiga -> nome, nome) == 0) {
            lseek(fd, -204, SEEK_CUR);
            write(fd, nova, sizeof(Pessoa));
            close(fd);
        }
    }
    close(fd);
}

int main(int argc, char ** argv) {

    return 0;
}
