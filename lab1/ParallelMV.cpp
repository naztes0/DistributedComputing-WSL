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
// Function for formatted matrix output
void printMatrix(double *pMatrix, int RowCount, int ColCount)
{
    for (int i = 0; i < RowCount; i++)
    {
        for (int j = 0; j < ColCount; j++)
            printf("%9.4f ", pMatrix[i * ColCount + j]);
        printf("\n");
    }
}
// Function for formatted vector output
void printVector(double *pVector, int Size)
{
    for (int i = 0; i < Size; i++)
        printf("%7.4f ", pVector[i]);
    printf("\n");
}

void serialResultCalculation(double *pMatrix, double *pVector, double *pResult, int size)
{
    for (int i = 0; i < size; i++)
    {
        pResult[i] = 0;
        for (int j = 0; j < size; j++)
            pResult[i] += pMatrix[i * size + j] * pVector[j];
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
// Function for distribution of the initial data among the processes
void dataDistribution(double *pMatrix, double *pProcRows, double *pVector, int size, int rowNum)
{
    MPI_Bcast(pVector, size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(pMatrix, rowNum * size, MPI_DOUBLE, pProcRows, rowNum * size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

void testDistribution(double *pMatrix, double *pVector, double *pProcRows,
                      int size, int rowNum)
{
    if (procRank == 0)
    {
        printf("Initial Matrix: \n");
        printMatrix(pMatrix, size, size);
        printf("Initial Vector: \n");
        printVector(pVector, size);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    for (int i = 0; i < procNum; i++)
    {
        if (procRank == i)
        {
            printf("\nProcRank = %d \n", procRank);
            printf(" Matrix Stripe:\n");
            printMatrix(pProcRows, rowNum, size);
            printf(" Vector: \n");
            printVector(pVector, size);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
}
// Process rows and vector mulriplication
void parallelResultCalculation(double *pProcRows, double *pVector, double *pProcResult, int size, int rowNum)
{
    for (int i = 0; i < rowNum; i++)
    {
        pProcResult[i] = 0;
        for (int j = 0; j < size; j++)
        {
            pProcResult[i] += pProcRows[i * size + j] * pVector[j];
        }
    }
}
// Function for testing the multiplication of matrix stripe and vector
void testPartialResults(double *pProcResult, int rowNum)
{
    int i; // Loop variable
    for (i = 0; i < procNum; i++)
    {
        if (procRank == i)
        {
            printf("ProcRank = %d \n", procRank);
            printf("Part of result vector: \n");
            printVector(pProcResult, rowNum);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
}
// Function for result vector replication
void resultReplication(double *pProcResult, double *pResult, int size, int rowNum)
{
    MPI_Allgather(pProcResult, rowNum, MPI_DOUBLE, pResult, rowNum, MPI_DOUBLE, MPI_COMM_WORLD);
}

// Testing the result of parallel matrix-vector multiplication
void testResult(double *pMatrix, double *pVector, double *pResult, int size)
{
    double *pSerialResult; // Result of serial matrix-vector multiplication
    int equal = 0;         // =0, if the serial and parallel results are identical
    int i;                 // Loop variable
    if (procRank == 0)
    {
        pSerialResult = new double[size];
        serialResultCalculation(pMatrix, pVector, pSerialResult, size);
        for (i = 0; i < size; i++)
        {
            if (pResult[i] != pSerialResult[i])
                equal = 1;
        }
        if (equal == 1)
            printf("The results of serial and parallel algorithms are NOT identical. Check your code.\n");
        else
            printf("The results of serial and parallel algorithms are identical.\n");
        delete[] pSerialResult;
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
    dataDistribution(pMatrix, pProcRows, pVector, size, rowNum);
    // Parallel matrix vector multiplication
    parallelResultCalculation(pProcRows, pVector, pProcResult, size, rowNum);
    // Result replication
    resultReplication(pProcResult, pResult, size, rowNum);
    testResult(pMatrix, pVector, pResult, size);
    processTermination(pMatrix, pVector, pResult, pProcRows, pProcResult);

    MPI_Finalize();

    return 0;
}