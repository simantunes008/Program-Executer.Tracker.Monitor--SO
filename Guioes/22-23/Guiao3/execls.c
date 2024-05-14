#include <stdio.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <sys/wait.h> /* chamadas wait*() e macros relacionadas */
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */

int main() {
    printf("before exec\n");
    execl("/bin/ls", "ls", "-l", NULL);
    printf("after exec"); /* A imagem do processo é substituída */
    return 0;
}
