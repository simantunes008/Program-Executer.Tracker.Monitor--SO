#include "execls.h"

int main() {
    printf("before exec\n");
    execl("/bin/ls", "ls", "-l", NULL);
    printf("after exec"); /* A imagem do processo é substituída */
    return 0;
}
