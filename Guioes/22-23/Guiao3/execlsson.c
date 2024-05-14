/**********************************************/
/** Diferentes formas de executar ls -l:     **/
/** execl("/bin/ls", "ls", "-l", NULL);      **/
/** execlp("ls", "ls", "-l", NULL);          **/
/** char * ls_argv[] = {"ls", "-l", NULL};   **/
/** execv("/bin/ls", ls_argv);               **/
/** execvp("ls", ls_argv);                   **/
/**********************************************/

#include <stdio.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <sys/wait.h> /* chamadas wait*() e macros relacionadas */
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */

int main() {
    printf("before exec\n");
    if (fork() == 0) {
        execl("/bin/ls", "ls", "-l", NULL);
        _exit(5); /* Caso o exec falhe o processo tem de parar */
    }
    int status;
    int terminated_process = wait(&status);
    printf("after exec | exit status: %d\n", WEXITSTATUS(status));
    return 0;
}
