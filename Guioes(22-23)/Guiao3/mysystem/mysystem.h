//
// Created by simao on 21-03-2023.
//

#ifndef MYSYSTEM_MYSYSTEM_H
#define MYSYSTEM_MYSYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <sys/wait.h> /* chamadas wait*() e macros relacionadas */
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */

int mysystemV1(const char * command);

int mysystemV2(const char * command);

#endif //MYSYSTEM_MYSYSTEM_H
