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

Pessoa * cria_pessoa(char * nome, int idade) {
    Pessoa * pessoa = malloc(sizeof(Pessoa));
    strncpy(pessoa -> nome, nome, 200);
    pessoa -> idade = idade;
    return pessoa;
}

void adiciona_pessoa(Pessoa * pessoa) {
    int pessoas_fd = open("pessoas.bin", O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (pessoas_fd < 0) return;
    write(pessoas_fd, pessoa, sizeof(Pessoa));
    close(pessoas_fd);
}

void lista_pessoas(int N) {
    int pessoas_fd = open("pessoas.bin", O_RDONLY);
    if (pessoas_fd < 0) return;
    Pessoa * pessoa = malloc(sizeof(Pessoa));
    int bytes_read = 0;
    char buffer[204];
    for (int i = 1; read(pessoas_fd, pessoa, sizeof(Pessoa)) > 0 && i <= N; i++) {
        bytes_read = snprintf(buffer, 204, "%s %d\n", pessoa->nome, pessoa->idade);
        write(STDOUT_FILENO, buffer, bytes_read);
    }
    close(pessoas_fd);
}

void altera_idade(Pessoa * nova) {
    int pessoas_fd = open("pessoas.bin", O_RDWR, 0644);
    if (pessoas_fd < 0) return;
    Pessoa * antiga = malloc(sizeof(Pessoa));
    while (read(pessoas_fd, antiga, sizeof(Pessoa)) > 0) {
        if (strcmp(antiga->nome, nova->nome) == 0) {
            lseek(pessoas_fd, -204, SEEK_CUR);
            write(pessoas_fd, nova, sizeof(Pessoa));
            free(antiga);
            break;
        }
    }
    close(pessoas_fd);
}

int main(int argc, char ** argv) {
    if (argc < 2) {
        write(STDOUT_FILENO, "Faltam argumentos!\n", 20);
        return 1;
    }

    if (!strcmp(argv[1], "-i") && argc == 4) {
        Pessoa * pessoa = cria_pessoa(argv[2], atoi(argv[3]));
        adiciona_pessoa(pessoa);
        free(pessoa);

    } else if (!strcmp(argv[1], "-l") && argc == 3) {
        lista_pessoas(atoi(argv[2]));

    } else if (!strcmp(argv[1], "-u") && argc == 4) {
        Pessoa * pessoa = cria_pessoa(argv[2], atoi(argv[3]));
        altera_idade(pessoa);
        free(pessoa);
        
    } else {
        write(STDOUT_FILENO, "Opção de utilização inválida!\n", 35);
    }
    return 0;
}
