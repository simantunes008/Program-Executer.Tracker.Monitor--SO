#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "person.h"

typedef struct person {
    char nome[200];
    int idade;
} Person;

Person * cria_pessoa(char * nome, int idade) {
    Person * pessoa = malloc(sizeof(Person));
    strncpy(pessoa -> nome, nome, 200);
    pessoa -> idade = idade;
    return pessoa;
}

int adiciona_pessoa(Person * pessoa) {
    int fd = open("pessoas.bin", O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (fd < 0) return -1;
    write(fd, pessoa, sizeof(Person));
    int posicao = lseek(fd, 0, SEEK_CUR) / sizeof(Person) - 1;
    close(fd);
    return posicao;
}

void lista_pessoas(int N) {
    int fd = open("pessoas.bin", O_RDONLY);
    if (fd < 0) return;
    Person * pessoa = malloc(sizeof(Person));
    int bytes = 0;
    char buffer[206];
    for (int i = 1; read(fd, pessoa, sizeof(Person)) > 0 && i <= N; i++) {
        bytes = snprintf(buffer, 206, "%s %d\n", pessoa -> nome, pessoa -> idade);
        write(STDOUT_FILENO, buffer, bytes);
    }
    close(fd);
}

void altera_idade(char * nome, int idade) {
    int fd = open("pessoas.bin", O_RDWR, 0644);
    if (fd < 0) return;
    Person * pessoa = malloc(sizeof(Person));
    while (read(fd, pessoa, sizeof(Person)) > 0) {
        if (strcmp(pessoa -> nome, nome) == 0) {
            pessoa -> idade = idade;
            lseek(fd, -sizeof(Person), SEEK_CUR);
            write(fd, pessoa, sizeof(Person));
            free(pessoa);
            break;
        }
    }
    close(fd);
}

void altera_idade_c_registo(int registo, int idade) {
    int fd = open("pessoas.bin", O_RDWR, 0644);
    if (fd < 0) return;
    Person * pessoa = malloc(sizeof(Person));
    lseek(fd, sizeof(Person) * registo, SEEK_SET);
    read(fd, pessoa, sizeof(Person));
    pessoa -> idade = idade;
    lseek(fd, -sizeof(Person), SEEK_CUR);
    write(fd, pessoa, sizeof(Person));
    free(pessoa);
    close(fd);
}
