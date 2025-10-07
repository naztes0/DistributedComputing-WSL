#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>

int procNum;  // Number of available processes
int procRank; // Rank of current process

// Initialization of data
void dummyDataInit(double *pMatrix, double *pVector, int size)
{
    for (int i = 0; i < size; i++)
    {
        pVector[i] = 1;
        for (int j = 0; j < size; j++)
        {
            pMatrix[i * size + j] = i;
        }
    }
}

// Function for memory allocation and data initialization
void processInitialization(double *&pMatrix, double *&pVector,
                           double *&pResult, double *&pProcRows, double *&pProcResult, int &size, int &rowNum)
{
    if (procRank == 0)
    {
        do
        {
            printf("\nEnter size of the matrix and vector: ");
            fflush(stdout);
            scanf("%d", &size);
            if (size < procNum)
            {
                printf("Size of the objects must be greater than "
                       "number of processes! \n ");
            }
            if (size % procNum != 0)
            {
                printf("Size of objects must be divisible by "
                       "number of processes! \n");
            }
        } while ((size < procNum) || (size % procNum != 0));
    }
    MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // Determine the number of matrix rows stored on each process
    rowNum = size / procNum;
    // Memory allocation
    pVector = new double[size];
    pResult = new double[size];
    pProcRows = new double[rowNum * size];
    pProcResult = new double[rowNum];
    // Obtain the values of initial data
    if (procRank == 0)
    {
        // Initial matrix exists only on the root process
        pMatrix = new double[size * size];
        // Values of elements are defined only on the root process
        dummyDataInit(pMatrix, pVector, size);
    }
}

void processTermination(double *pMatrix, double *pVector, double *pResult, double *pProcRows, double *pProcResult)
{
    if (procRank == 0)
        delete[] pMatrix;
    delete[] pVector;
    delete[] pResult;
    delete[] pProcRows;
    delete[] pProcResult;
}
int main(int argc, char *argv[])
{
    double *pMatrix;     // First argument - initial matrix
    double *pVector;     // Second argument - initial vector
    double *pResult;     // Result vector for matrix-vector multiplication
    double *pProcRows;   // Stripe of the matrix on current process
    double *pProcResult; // Block of result vector on current process
    int rowNum;          // Number of rows in matrix stripe
    int size;            // Sizes of initial matrix and vector
    double start, finish, duration;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
    if (procRank == 0)
        printf("Parallel matrix - vector multiplication program\n");

    processInitialization(pMatrix, pVector, pResult, pProcRows, pProcResult, size, rowNum);

    processTermination(pMatrix, pVector, pResult, pProcRows, pProcResult);
    MPI_Finalize();

    return 0;
}