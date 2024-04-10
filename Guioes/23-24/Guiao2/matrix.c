#include "matrix.h"

int **createMatrix() {

    // seed random numbers
    srand(time(NULL));

    // Allocate and populate matrix with random numbers.
    printf("Generating numbers from 0 to %d...", MAX_RAND);
    int **matrix = (int **) malloc(sizeof(int*) * ROWS);
    for (int i = 0; i < ROWS; i++) {
        matrix[i] = (int*) malloc(sizeof(int) * COLUMNS);
        for (int j = 0; j < COLUMNS; j++) {
            matrix[i][j] = rand() % MAX_RAND;
        }
    }
    printf("Done.\n");

    return matrix;
}

void printMatrix(int **matrix) {

    for (int i = 0; i < ROWS; i++) {
        printf("%2d | ", i);
        for (int j = 0; j < COLUMNS; j++) {
            printf("%7d ", matrix[i][j]);
        }
        printf("\n");
    }
}

// ex.5
int valueExists(int **matrix, int value) {

    int status, result = 0;

    for (int i = 0; i < ROWS; i++) {
        if (fork() == 0) {
            for (int j = 0; j < COLUMNS; j++) {
                if (matrix[i][j] == value) {
                    _exit(1);
                }
            }
            _exit(0);
        }
    }

    for (int i = 0; i < ROWS; i++) {
        wait(&status);
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status)) {
                result = 1;
            }
        } else {
            printf("[father] bad exit\n");
        }
    }

    return result;
}

// ex.6
void linesWithValue(int **matrix, int value) {

    pid_t pid;
    int status;
    
    for (int i = 0; i < ROWS; i++) {
        if (fork() == 0) {
            for (int j = 0; j < COLUMNS; j++) {
                if (matrix[i][j] == value) {
                    _exit(i);
                }
            }
            _exit(-1);
        }
        pid = wait(&status);
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) != 255) {
                printf("[father] child process: %d found it in line %d!\n", pid, WEXITSTATUS(status));
            }
        } else {
            printf("[father] bad exit\n");
        }
    }
}
