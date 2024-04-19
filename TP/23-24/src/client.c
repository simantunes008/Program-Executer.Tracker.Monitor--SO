#include "../include/util.h"

void execute(int time, char* prog, char* cmd) {
    int fd1 = open("tmp/stats", O_WRONLY);
    if (fd1 == -1) {
		perror("Failed to open FIFO\n");
        return;
	}
    
    pid_t pid = getpid();

    Task t;
    t.pid = pid;
    t.time = time;
    strcpy(t.cmd, cmd);
    strcpy(t.prog, prog);
    t.finished = false;

    char s_pid[20];

	sprintf(s_pid, "tmp/%d", pid);

    if (mkfifo(s_pid, 0777) == -1) {
        if (errno != EEXIST) {
            perror("Could not create fifo file\n");
            return;
        }
    }

    write(fd1, &t, sizeof(t));

    close(fd1);

    int fd2 = open(s_pid, O_RDONLY);
    if (fd2 == -1) {
		perror("Failed to open FIFO\n");
        return;
	}

    int t_id;

    read(fd2, &t_id, sizeof(int));

    printf("TASK %d Received\n", t_id); // ! Mudar este printf para um write

    close(fd2);

    unlink(s_pid);
}


void status() {
    int fd1 = open("tmp/stats", O_WRONLY);
    if (fd1 == -1) {
		perror("Failed to open FIFO\n");
        return;
	}

    pid_t pid = getpid();
	char s_pid[20];

	sprintf(s_pid, "tmp/%d", pid);

    if (mkfifo(s_pid, 0777) == -1) {
        if (errno != EEXIST) {
            perror("Could not create fifo file\n");
            return;
        }
    }

    Task t;
    t.pid = pid;
    t.time = 0;
    strcpy(t.cmd, "status");
    strcpy(t.prog, "");
    t.finished = false;

    write(fd1, &t, sizeof(t));

    close(fd1);

    int fd2 = open(s_pid, O_RDONLY);
    if (fd2 == -1) {
        perror("Failed to open FIFO\n"); 
        return;
	}

    int res;
    Entry e;

    // printf("Completed\n");
    while ((res = read(fd2, &e, sizeof(e))) > 0) {
        if (e.texec) {
            printf("%d %s %ld ms\n", e.pid, e.prog, e.texec); // ! Mudar este printf para um write
        } else {
            printf("%d %s\n", e.pid, e.prog); // ! Mudar este printf para um write
        }
	    
    }

    close(fd2);

    unlink(s_pid);
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
    
    if (!strcmp(argv[1], "execute") && argc == 5 && atoi(argv[2]) > 0) {
        char cmd[20];
        sprintf(cmd, "%s %s", argv[1], argv[3]);
        
        if (!strcmp(argv[3], "-u") || !strcmp(argv[3], "-p")) {
            execute(atoi(argv[2]), argv[4], cmd);

        } else {
            printf("Invalid option\n");
        }

    } else if (!strcmp(argv[1], "status") && argc == 2) {
        status();


    } else {
        printf("Invalid command name or count.\n");
    }

    return 0;
}