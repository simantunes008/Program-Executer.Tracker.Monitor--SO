#include "matrix.h"

int main(int argc, char *argv[]) {

    // generate random matrix
    int **matrix = createMatrix();

    // print matrix
    printMatrix(matrix);

    // TO DO
    if (valueExists(matrix, 4)) {
        linesWithValue(matrix, 4);    
    } else {
        printf("There's no such value.\n");
    }

    // free matrix
    for (int i = 0; i < ROWS; i++) {
        free(matrix[i]);
    }
    free(matrix);

    return 0;
}