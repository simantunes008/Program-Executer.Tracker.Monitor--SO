#include "../include/util.h"
#include <glib.h>
#define QUANTUM 100

void fWriter(int pipe) {
    int fd = open("history.bin", O_CREAT | O_APPEND | O_WRONLY, 0777);
	if(fd == -1) {
        perror("Failed to open file!\n");
        return;
    }

    int res;
    Entry e;

    while ((res = read(pipe, &e, sizeof(e))) > 0) {
        write(fd, &e, res);
    }
}


void manager(int pipes[2], char* folder, int max_parallel_tasks) {

    mkdir(folder, 0777);

    int pipe_fd1[2];
    int i = 0;

    if (pipe(pipe_fd1) == -1){
		perror("Failed to create pipe to file writer!\n");
        return;
	}

    if (fork() == 0) {
        close(pipe_fd1[1]);
		fWriter(pipe_fd1[0]);
    }

    close(pipe_fd1[0]);

    int res;
    Task t;

    while ((res = read(pipes[0], &t, sizeof(t))) > 0) {

        t.time -= QUANTUM;

        if (t.finished) {
            i--;
            continue;
        }

        if (t.time <= 0 && i < max_parallel_tasks) {
            i++;

            // TODO: Adcionar prioridade às tarefas que estão à espera de um filho

            if (fork() == 0) {
                struct timeval start, end;

                char out_file[20];
                sprintf(out_file, "%s/%d.txt", folder, t.pid);

                if (!strcmp(t.cmd, "execute -u")) {
                    gettimeofday(&start, NULL);
                    mysystem(t.prog, out_file);
                    gettimeofday(&end, NULL);
                } 
                if (!strcmp(t.cmd, "execute -p")) {
                    gettimeofday(&start, NULL);
                    pipeline(t.prog, out_file);
                    gettimeofday(&end, NULL);
                }

                long int texec = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;

                Entry e;
                e.pid = t.pid;
                e.texec = texec;
                strcpy(e.prog, t.prog);

                write(pipe_fd1[1], &e, sizeof(e));

                int massa = open("tmp/stats", O_WRONLY); // ! Mudar o nome deste descritor
                if (massa == -1) {
    	        	perror("Failed to open stats FIFO\n");
    	        }

                t.finished = true;

                write(massa, &t, sizeof(t));
                write(pipes[1], &t, sizeof(t));
            }
            
        } else {
            write(pipes[1], &t, sizeof(t));

        }
    }
}


int main(int argc, char ** argv) {

    if (mkfifo("tmp/stats", 0777) == -1) {
        if (errno != EEXIST) {
            perror("Could not create fifo file\n");
            return 1;
        }
    }

    int child_pipe[2];

    if (pipe(child_pipe) == -1){
		perror("Failed to create pipe to file writer!\n");
        return 1;
	}

    int parallel_tasks = atoi(argv[2]);

    if (argc == 3 && parallel_tasks > 0) {
        if (fork() == 0) {
            manager(child_pipe, argv[1], parallel_tasks);

        }
    } else {
        printf("Invalid command name or count.\n");
    }

    close(child_pipe[0]);

    GHashTable *pending_tasks = g_hash_table_new(g_direct_hash, g_direct_equal);

    // ! Cuidado que bloqueia
    int fd1 = open("tmp/stats", O_RDONLY);
    if (fd1 == -1) {
		perror("Failed to open stats FIFO\n");
        return 1;
	}

    int fd2 = open("tmp/stats", O_WRONLY);
    if (fd2 == -1) {
		perror("Failed to open stats FIFO\n");
        return 1;
	}

    int res;
    Task t;

    while ((res = read(fd1, &t, sizeof(t))) > 0) {
    
        if (!strcmp(t.cmd, "execute -u") || !strcmp(t.cmd, "execute -p")) {

            if (g_hash_table_contains(pending_tasks, GINT_TO_POINTER(t.pid))) {
                g_hash_table_remove(pending_tasks, GINT_TO_POINTER(t.pid));
                continue;
            }

            char s_pid[20];
            sprintf(s_pid, "tmp/%d", t.pid);

            int fd2 = open(s_pid, O_WRONLY);
            if (fd2 == -1) {
    	        perror("Failed to open s_pid FIFO\n");
    	    }

            Entry *e = malloc(sizeof(struct entry));
            e -> pid = t.pid;
            e -> texec = 0;
            strcpy(e -> prog, t.prog);

            g_hash_table_insert(pending_tasks, GINT_TO_POINTER(e -> pid), e);

            write(fd2, &t.pid, sizeof(int));

            close(fd2);
            unlink(s_pid);

            write(child_pipe[1], &t, sizeof(t));

        } else if (!strcmp(t.cmd, "status")) {

            char s_pid[20];

	        sprintf(s_pid, "tmp/%d", t.pid);

            int massa = open(s_pid, O_WRONLY); // ! Mudar o nome deste descritor
            if (massa == -1) {
	        	perror("Failed to open FIFO\n");
                return 1;
	        }

            GHashTableIter iter;
    	    gpointer key, value;

            g_hash_table_iter_init(&iter, pending_tasks);

            while (g_hash_table_iter_next(&iter, &key, &value)) {
        	    Entry *sopa = (Entry *)value;
                write(massa, sopa, sizeof(struct entry));
    	    }
            
            int sopa = open("history.bin", O_RDONLY, 0777); // ! Mudar o nome deste descritor
	        if(sopa == -1) {
                perror("Failed to open file exec stats!\n");
            }

            int res;
            Entry e;

            while ((res = read(sopa, &e, sizeof(e))) > 0) {
                write(massa, &e, sizeof(e));
            }

            close(massa);
            unlink(s_pid);
        }
    }

    return 0;
}