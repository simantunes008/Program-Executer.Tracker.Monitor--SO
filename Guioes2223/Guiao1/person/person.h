//
// Created by simao on 28-02-2023.
//

#ifndef PERSON_PERSON_H
#define PERSON_PERSON_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int new_person(char * name, int age);

int person_change_ageV1(char * name, int age);

void person_change_ageV2(int regestry, int age);

#endif //PERSON_PERSON_H
