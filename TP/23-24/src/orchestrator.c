#include "../include/util.h"


void fWriter(int pipe) {

    int fd = open("history.bin", O_CREAT | O_APPEND | O_WRONLY, 0777);
	if(fd == -1) {
        perror("Failed to open file exec stats!\n");
        return;
    }

    int res;
    Entry e;

    while ((res = read(pipe, &e, sizeof(e))) > 0) {
        write(fd, &e, res);
    }
}


int main(int argc, char ** argv) {

    if (argc == 1) {
		printf("Invalid Input\n");
		return 0;
	}

    if (mkfifo("tmp/stats", 0777) == -1) {
        if (errno != EEXIST) {
            perror("Could not create fifo file\n");
            return 1;
        }
    }

    int pipe_fd[2];
    close(pipe_fd[0]);

    if (pipe(pipe_fd) == -1){
		perror("Failed to create pipe to file writer!\n");
        return 1;
	}

    if (argc == 2) {
        mkdir("files", 0777); // ! Se não der o nome da pasta fica a 'files'

		if (fork() == 0) {
            close(pipe_fd[1]);
			fWriter(pipe_fd[0]);

		}
	} else if (argc == 3) {
        mkdir(argv[1], 0777);

		if (fork() == 0) {
            close(pipe_fd[1]);
		    fWriter(pipe_fd[0]);
            
		}
	} else {
		perror("Invalid command number!");  
        return 1;
	}

    int fifo_fd1 = open("tmp/stats", O_RDONLY);
    if (fifo_fd1 == -1) {
		perror("Failed to open FIFO\n");
        return 1;
	}

    int fifo_fd2 = open("tmp/stats", O_WRONLY);
    if (fifo_fd2 == -1) {
		perror("Failed to open FIFO\n");
        return 1;
	}

    int res;
    Task t;

    while ((res = read(fifo_fd1, &t, sizeof(t))) > 0) {
    
        if (!strcmp(t.cmd, "execute")) {
            struct timeval start, end;

            char s_pid[20];
            char out_file[20];

	        sprintf(s_pid, "tmp/%d", t.pid);
            sprintf(out_file, "files/%d.txt", t.pid);

            gettimeofday(&start, NULL);
            mysystem(t.prog, out_file);
            gettimeofday(&end, NULL);

            long int duration = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;

            int fd2 = open(s_pid, O_WRONLY);
            if (fd2 == -1) {
	        	perror("Failed to open FIFO\n");
                return 1;
	        }

            write(fd2, &t.pid, sizeof(int));

            close(fd2);
            unlink(s_pid);

            Entry e;
            e.pid = t.pid;
            e.texec = duration;
            strcpy(e.prog, t.prog);

            write(pipe_fd[1], &e, sizeof(e));

        } else if (!strcmp(t.cmd, "status")) {
            
            int cona = open("history.bin", O_RDONLY, 0777);
	        if(cona == -1) {
                perror("Failed to open file exec stats!\n");
            }

            char s_pid[20];

	        sprintf(s_pid, "tmp/%d", t.pid);

            int pila = open(s_pid, O_WRONLY);
            if (pila == -1) {
	        	perror("Failed to open FIFO\n");
                return 1;
	        }

            int res;
            Entry e;


            while ((res = read(cona, &e, sizeof(e))) > 0) {
                write(pila, &e, sizeof(e));
            }

            close(pila);
            unlink(s_pid);
        }
    }

    return 0;
}