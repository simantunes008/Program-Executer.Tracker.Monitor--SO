#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "person.h"

int main(int argc, char* argv[]){
    if (argc < 3) {
        printf("Usage:\n");
        printf("Add new person: ./pessoas -i [name] [age]\n");
        printf("List N persons: ./pessoas -l [N]\n");
        printf("Change person age: ./pessoas -u [name] [age]\n");
        printf("Change person age (v2): ./pessoas -o [position] [age]\n");
        return 1;
    }
    if (strcmp(argv[1],"-i") == 0) {
        Person * pessoa = cria_pessoa(argv[2], atoi(argv[3]));
        printf("registo %d\n", adiciona_pessoa(pessoa));
        free(pessoa);
    }
    if (strcmp(argv[1],"-l") == 0) {
        lista_pessoas(atoi(argv[2]));
    }
    if (strcmp(argv[1],"-u") == 0 ) {
        altera_idade(argv[2], atoi(argv[3]));
    }
    if (strcmp(argv[1],"-o") == 0 ) {
        altera_idade_c_registo(atoi(argv[2]), atoi(argv[3]));
    }
    return 0;
}
