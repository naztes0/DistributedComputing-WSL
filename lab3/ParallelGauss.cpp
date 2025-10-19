#include <iostream>
#include <mpi.h>

int ProcNum = 0;  // Number of the available processes
int ProcRank = 0; // Rank of the current process
int *pProcInd;    // Number of the first row located on the processes
int *pProcNum;    // Number of the linear system rows located on the processes

void DummyDataInitialization(double *pMatrix, double *pVector, int Size)
{
    int i, j; // Loop variables
    for (i = 0; i < Size; i++)
    {
        pVector[i] = i + 1;
        for (j = 0; j < Size; j++)
        {
            if (j <= i)
                pMatrix[i * Size + j] = 1;
            else
                pMatrix[i * Size + j] = 0;
        }
    }
}

void RandomDataInitialization(double *pMatrix, double *pVector, int Size)
{
    int i, j; // Loop variables
    srand(unsigned(clock()));
    for (i = 0; i < Size; i++)
    {
        pVector[i] = rand() / double(10000000);
        for (j = 0; j < Size; j++)
        {
            pMatrix[i * Size + j] = rand() / double(1000000);
        }
    }
}

void PrintMatrix(double *pMatrix, int RowCount, int ColCount)
{
    for (int i = 0; i < RowCount; i++)
    {
        for (int j = 0; j < ColCount; j++)
            printf("%9.4f ", pMatrix[i * ColCount + j]);
        printf("\n");
    }
}

void PrintVector(double *pVector, int Size)
{
    for (int i = 0; i < Size; i++)
        printf("%7.4f ", pVector[i]);
    printf("\n");
}
// Function for memory allocation and data initialization
void ProcessInitialization(double *&pMatrix, double *&pVector, double *&pResult, double *&pProcRows, double *&pProcVector, double *&pProcResult, int &Size, int &RowNum)
{
    if (ProcRank == 0)
    {
        do
        {
            printf("\nEnter the size of the matrix and the vector: ");
            scanf("%d", &Size);
            if (Size < ProcNum)
            {
                printf("Size must be greater than numbфer of processes! \n");
            }
        } while (Size < ProcNum);
    }
    MPI_Bcast(&Size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int RestRows = Size;
    for (int i = 0; i < ProcRank; i++)
        RestRows = RestRows - RestRows / (ProcNum - i);
    RowNum = RestRows / (ProcNum - ProcRank);
    pProcRows = new double[RowNum * Size];
    pProcVector = new double[RowNum];
    pProcResult = new double[RowNum];
    pProcInd = new int[ProcNum];
    pProcNum = new int[ProcNum];
    if (ProcRank == 0)
    {
        pMatrix = new double[Size * Size];
        pVector = new double[Size];
        pResult = new double[Size];
        DummyDataInitialization(pMatrix, pVector, Size);
    }
}

// Function for the data distribution among the processes
void DataDistribution(double *pMatrix, double *pProcRows, double *pVector, double *pProcVector, int Size, int RowNum)
{
    int *pSendNum;       // Number of the elements sent to the process
    int *pSendInd;       // Index of the first data element sent to the process
    int RestRows = Size; // Number of rows, that have not been distributed yet

    pSendInd = new int[ProcNum];
    pSendNum = new int[ProcNum];
    // Define the disposition of the matrix rows for the current process
    RowNum = (Size / ProcNum);
    pSendNum[0] = RowNum * Size;
    pSendInd[0] = 0;
    for (int i = 1; i < ProcNum; i++)
    {
        RestRows -= RowNum;
        RowNum = RestRows / (ProcNum - i);
        pSendNum[i] = RowNum * Size;
        pSendInd[i] = pSendInd[i - 1] + pSendNum[i - 1];
    }
    // Scatter the rows
    MPI_Scatterv(pMatrix, pSendNum, pSendInd, MPI_DOUBLE, pProcRows, pSendNum[ProcRank], MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Define the disposition of the matrix rows for current process
    RestRows = Size;
    pProcInd[0] = 0;
    pProcNum[0] = Size / ProcNum;
    for (int i = 1; i < ProcNum; i++)
    {
        RestRows -= pProcNum[i - 1];
        pProcNum[i] = RestRows / (ProcNum - i);
        pProcInd[i] = pProcInd[i - 1] + pProcNum[i - 1];
    }

    MPI_Scatterv(pVector, pProcNum, pProcInd, MPI_DOUBLE, pProcVector, pProcNum[ProcRank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
    // Free the memory
    delete[] pSendNum;
    delete[] pSendInd;
}

// Function for testing the data distribution
void TestDistribution(double *pMatrix, double *pVector, double *pProcRows, double *pProcVector, int Size, int RowNum)
{
    if (ProcRank == 0)
    {
        printf("Initial Matrix: \n");
        PrintMatrix(pMatrix, Size, Size);
        printf("Initial Vector: \n");
        PrintVector(pVector, Size);
    }
    for (int i = 0; i < ProcNum; i++)
    {
        if (ProcRank == i)
        {
            printf("\nProcRank = %d \n", ProcRank);
            printf(" Matrix Stripe:\n");
            PrintMatrix(pProcRows, RowNum, Size);
            printf(" Vector: \n");
            PrintVector(pProcVector, RowNum);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
}

// Function for computational process termination
void ProcessTermination(double *pMatrix, double *pVector, double *pResult, double *pProcRows, double *pProcVector, double *pProcResult)
{
    if (ProcRank == 0)
    {
        delete[] pMatrix;
        delete[] pVector;
        delete[] pResult;
    }
    delete[] pProcRows;
    delete[] pProcVector;
    delete[] pProcResult;
    delete[] pProcInd;
    delete[] pProcNum;
}
int main(int argc, char *argv[])
{
    double *pMatrix;     // Matrix of the linear system
    double *pVector;     // Right parts of the linear system
    double *pResult;     // Result vector
    double *pProcRows;   // Rows of the matrix A
    double *pProcVector; // Block of the vector b
    double *pProcResult; // Block of the vector x
    int Size;            // Size of the matrix and vectors
    int RowNum;          // Number of the matrix rows
    double Start, Finish, Duration;

    setvbuf(stdout, 0, _IONBF, 0);
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    if (ProcRank == 0)
    {
        printf("Parallel Gauss algorithm for solving linear systems\n");
    }
    ProcessInitialization(pMatrix, pVector, pResult, pProcRows, pProcVector, pProcResult, Size, RowNum);
    if (ProcRank == 0)
    {
        printf("Initial matrix \n");
        PrintMatrix(pMatrix, Size, Size);
        printf("Initial vector \n");
        PrintVector(pVector, Size);
    }
    DataDistribution(pMatrix, pProcRows, pVector, pProcVector, Size, RowNum);
    TestDistribution(pMatrix, pVector, pProcRows, pProcVector, Size, RowNum);
    ProcessTermination(pMatrix, pVector, pResult, pProcRows, pProcVector, pProcResult);

    MPI_Finalize();
}