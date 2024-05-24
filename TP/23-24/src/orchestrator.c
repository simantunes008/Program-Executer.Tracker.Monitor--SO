#include "../include/util.h"
#include <glib.h>
#define QUANTUM 100

void fWriter(int pipe) {
    int fd = open("history.bin", O_CREAT | O_APPEND | O_WRONLY, 0777);
	if (fd == -1) {
        perror("Failed to open history file\n");
        return;
    }

    int res;
    Entry e;

    while ((res = read(pipe, &e, sizeof(e))) > 0) {
        write(fd, &e, res);
    }
}


void executer(int ready_queue, int pipe, char* folder) {
    int res;
    Task t;

    while ((res = read(ready_queue, &t, sizeof(t))) > 0) {

        struct timeval start, end;

        char out_file[20];
        sprintf(out_file, "%s/%d.txt", folder, t.pid);

        gettimeofday(&start, NULL);
        if (!strcmp(t.cmd, "execute -u")) execute_u(t.prog, out_file);
        else execute_p(t.prog, out_file);
        gettimeofday(&end, NULL);

        long int texec = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;

        Entry e;
        e.pid = t.pid;
        e.texec = texec;
        strcpy(e.prog, t.prog);

        write(pipe, &e, sizeof(e));

        int fd = open("tmp/stats", O_WRONLY);
        if (fd == -1) {
	    	perror("Failed to open FIFO stats\n");
            return;
	    }

        write(fd, &t, sizeof(t));
    }
}


void scheduler(int job_queue[2], char* folder, int max_parallel_tasks) {

    mkdir(folder, 0777);

    int writer_pipe[2];
    if (pipe(writer_pipe) == -1){
		perror("Failed to create pipe to file writer\n");
        return;
	}

    if (fork() == 0) {
        close(writer_pipe[1]);
		fWriter(writer_pipe[0]);
    }

    int ready_queue[2];

    if (pipe(ready_queue) == -1) {
        perror("Failed to create pipe to child processes\n");
        return;
    }

    for (int i = 0; i < max_parallel_tasks; i++) {
        if (fork() == 0) {
            close(ready_queue[1]);
            executer(ready_queue[0], writer_pipe[1], folder);
        }
    }

    close(ready_queue[0]);
    close(writer_pipe[0]);

    int res;
    Task t;

    while ((res = read(job_queue[0], &t, sizeof(t))) > 0) {
        t.time -= QUANTUM;
        if (t.time <= 0) write(ready_queue[1], &t, sizeof(t));
        else write(job_queue[1], &t, sizeof(t));
    }
}


int main(int argc, char ** argv) {

    if (mkfifo("tmp/stats", 0777) == -1) {
        if (errno != EEXIST) {
            perror("Could not create FIFO stats\n");  // ! Verifica se o FIFO já foi criado
            return 1;
        }
    }

    int job_queue[2];

    if (pipe(job_queue) == -1){
		perror("Failed to create pipe to handler\n");
        return 1;
	}

    int parallel_tasks = atoi(argv[2]);

    if (argc == 3 && parallel_tasks > 0) {
        if (fork() == 0) {
            scheduler(job_queue, argv[1], parallel_tasks);
        }
    } else {
        printf("Invalid command name or count\n");
    }

    close(job_queue[0]);

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

        if (g_hash_table_contains(pending_tasks, GINT_TO_POINTER(t.pid))) {
            g_hash_table_remove(pending_tasks, GINT_TO_POINTER(t.pid));
            continue;
        }

        char s_pid[20];
        sprintf(s_pid, "tmp/%d", t.pid);

        int fd3 = open(s_pid, O_WRONLY);
        if (fd3 == -1) {
    	    perror("Failed to open FIFO\n");
            return 1;
    	}
    
        if (!strcmp(t.cmd, "execute -u") || !strcmp(t.cmd, "execute -p")) {

            Entry *e = malloc(sizeof(struct entry));
            e -> pid = t.pid;
            e -> texec = 0;
            strcpy(e -> prog, t.prog);

            g_hash_table_insert(pending_tasks, GINT_TO_POINTER(e -> pid), e);

            write(fd3, &t.pid, sizeof(int));
            write(job_queue[1], &t, sizeof(t));

        } else if (!strcmp(t.cmd, "status")) {

            GHashTableIter iter;
    	    gpointer key, value;

            g_hash_table_iter_init(&iter, pending_tasks);

            while (g_hash_table_iter_next(&iter, &key, &value)) {
        	    Entry *e = (Entry *)value;
                write(fd3, e, sizeof(struct entry));
    	    }
            
            int history_fd = open("history.bin", O_RDONLY, 0777);
	        if (history_fd == -1) {
                perror("Failed to open history file\n");
                return 1;
            }

            int res;
            Entry e;

            while ((res = read(history_fd, &e, sizeof(e))) > 0) {
                write(fd3, &e, sizeof(e));
            }
        }

        close(fd3);
        unlink(s_pid);
    }

    return 0;
}