#include "person.h"

typedef struct Person{
    char name[200];
    int age;
} Person;

int new_person(char * name, int age) {
    int fd = open("people", O_CREAT | O_WRONLY | O_APPEND, 0640);
    Person p;
    strncpy(p.name, name, 200);
    p.age = age;
    write(fd, &p, sizeof(Person));
    return lseek(fd, 0, SEEK_CUR) / sizeof(Person);
}

int person_change_ageV1(char * name, int age) {
    int fd = open("people", O_RDWR);
    Person p1, p2;
    strncpy(p1.name, name, 200);
    p1.age = age;
    while (read(fd, &p2, sizeof(Person)) > 0) {
        if (strcmp(p2.name, name) == 0) {
            lseek(fd, -204, SEEK_CUR);
            write(fd, &p1, sizeof(Person));
            close(fd);
            return 0;
        }
    }
    close(fd);
    return -1;
}

void person_change_ageV2(int regestry, int age) {
    int fd = open("people", O_RDWR);
    Person p;
    lseek(fd, (regestry - 1) * sizeof(Person), SEEK_SET);
    read(fd, &p, sizeof(Person));
    p.age = age;
    lseek(fd, (regestry - 1) * sizeof(Person), SEEK_SET);
    write(fd, &p, sizeof(Person));
    close(fd);
}

int main(int argc, char * argv[]) {
    if (argc == 1) {
        char buf[204];
        int read_bytes;
        Person p;
        int fd = open("people", O_RDONLY);
        while(read(fd, &p, sizeof(Person)) > 0) {
            read_bytes = snprintf(buf, 204, "%s %d\n", p.name, p.age);
            write(STDOUT_FILENO, buf, read_bytes);
        }
        return 0;
    }
    if (argc == 4 && strcmp(argv[1],"-i") == 0) {
        char buf[24];
        int read_bytes = snprintf(buf, 24, "registo %d\n", new_person(argv[2], atoi(argv[3])));
        write(STDOUT_FILENO, buf, read_bytes);
    }
    if (argc == 4 && strcmp(argv[1],"-u") == 0) {
        int n = person_change_ageV1(argv[2], atoi(argv[3]));
        if (n == -1) write(STDOUT_FILENO, "Erro: Pessoa inexistente\n", 25);
    }
    if (argc == 4 && strcmp(argv[1],"-o") == 0) {
        person_change_ageV2(atoi(argv[2]), atoi(argv[3]));
    }
}
