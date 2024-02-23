#ifndef PERSON_H
#define PERSON_H

typedef struct person Person;

Person * cria_pessoa(char * nome, int idade);

int adiciona_pessoa(Person * pessoa);

void lista_pessoas(int N);

void altera_idade(char * nome, int nova_idade);

void altera_idade_c_registo(int registo, int nova_idade);

#endif
