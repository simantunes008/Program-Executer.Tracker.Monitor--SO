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
        bytes_read = snprintf(buffer, 204, "%s %d\n", pessoa->nome, pessoa->idade);
        write(STDOUT_FILENO, buffer, bytes_read);
    }
    close(fd);
}

void altera_idade(Pessoa * nova) {
    int fd = open("pessoas.bin", O_RDWR, 0644);
    if (fd < 0) return;
    Pessoa * antiga = malloc(sizeof(Pessoa));
    while (read(fd, antiga, sizeof(Pessoa)) > 0) {
        if (strcmp(antiga->nome, nova->nome) == 0) {
            lseek(fd, -204, SEEK_CUR);
            write(fd, nova, sizeof(Pessoa));
            break;
        }
    }
    close(fd);
}

int main(int argc, char ** argv) {
    if (argc < 2) {
        write(STDOUT_FILENO, "Faltam argumentos!\n", 20);
        return 1;
    }

    if (!strcmp(argv[1], "-i") && argc == 4) {
        Pessoa * pessoa = malloc(sizeof(Pessoa));
        strncpy(pessoa -> nome, argv[2], 200);
        pessoa -> idade = atoi(argv[3]);
        adiciona_pessoa(pessoa);

    } else if (!strcmp(argv[1], "-l") && argc == 3) {
        listar_pessoas(atoi(argv[2]));

    } else if (!strcmp(argv[1], "-u") && argc == 4) {
        Pessoa * pessoa = malloc(sizeof(Pessoa));
        strncpy(pessoa -> nome, argv[2], 200);
        pessoa -> idade = atoi(argv[3]);
        altera_idade(pessoa);
        
    } else {
        write(STDOUT_FILENO, "Opção de utilização inválida!\n", 35);
    }
    return 0;
}
