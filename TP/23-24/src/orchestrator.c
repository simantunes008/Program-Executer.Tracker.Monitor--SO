#include "../include/util.h"





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

    int pipe_fd[2];
    close(pipe_fd[0]);

    if (pipe(pipe_fd) == -1){
		perror("Failed to create pipe to file writer!\n");
        return 1;
	}

    if (argc == 2) {
		if (fork() == 0) {
			close(pipe_fd[1]);
			// TODO


		}
	} else if (argc == 3) {
		if (fork() == 0) {
            close(pipe_fd[1]);
		    // TODO

            
		}
	} else {
		perror("Invalid command number!");
        return 1;
	}

    int res;
    Task temp;

    while ((res = read(fifo_fd1, &temp, sizeof(temp))) > 0) {
    
        if (!strcmp(temp.cmd, "execute")) {
            struct timeval start, end;

            char s_pid[20];
            char out_file[20];

	        sprintf(s_pid, "tmp/%d", temp.pid);
            sprintf(out_file, "files/%d.txt", temp.pid);

            gettimeofday(&start, NULL);
            mysystem(temp.prog, out_file);
            gettimeofday(&end, NULL);

            long int texec = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;

            int fd2 = open(s_pid, O_WRONLY);
            if (fd2 == -1) {
	        	perror("Failed to open FIFO\n");
                return 1;
	        }

            write(fd2, &temp.pid, sizeof(int));

            close(fd2);

            unlink(s_pid);
        } else if (!strcmp(temp.cmd, "status")) {
            printf("ainda não fiz\n");
            // TODO


        }
    }



    return 0;
}