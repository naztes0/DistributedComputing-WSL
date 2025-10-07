#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    double *pMatrix; // First argument - initial matrix
    double *pVector; // Second argument - initial vector
    double *pResult; // Result vector for matrix-vector multiplication
    int Size;        // Sizes of initial matrix and vector
    double Start, Finish, Duration;
    MPI_Init(&argc, &argv);
    printf("Parallel matrix - vector multiplication program\n");
    MPI_Finalize();

    return 0;
}