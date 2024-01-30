//
// Created by simao on 23-02-2023.
//

#ifndef MYNL_MYNL_H
#define MYNL_MYNL_H

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h> /* O_RDONLY, O_WRONLY, O_CREAT, O_* */

int readch(int fd, char * buf);

ssize_t readln (int fd, char *line, size_t size);

int mynl();

#endif //MYNL_MYNL_H
