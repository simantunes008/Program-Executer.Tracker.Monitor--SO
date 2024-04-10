#include "mysystem.h"


// recebe um comando por argumento
// returna -1 se o fork falhar
// caso contrario retorna o valor do comando executado
int mysystem (const char* command) {

	int res = 0, arg_count = 0;
	char * arg_values[MAX_ARGS];

    char * token = strtok((char *)command, " ");
    while (token != NULL && arg_count < MAX_ARGS - 1) {
        arg_values[arg_count++] = token;
        token = strtok(NULL, " ");
    }
    arg_values[arg_count] = NULL;
	
    if (fork() == 0) {

		execvp(arg_values[0], arg_values);
		// ! O professor disse para não fazer assim
		//const char * args[] = {"/bin/sh", "-c", command, NULL};
		//execv(args[0], (char * const *)args);

		_exit(1); 
    }

	wait(&res);
	if (WIFEXITED(res)) res = -WEXITSTATUS(res);

	return res;
}