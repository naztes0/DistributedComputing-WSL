#include <iostream>

// Function for simple initialization of the matrix and the vector elements
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
// Function for random initialization of the matrix and the vector elements
void RandomDataInitialization(double *pMatrix, double *pVector, int Size)
{
    int i, j; // Loop variables
    srand(unsigned(clock()));
    for (i = 0; i < Size; i++)
    {
        pVector[i] = rand() / double(1000);
        for (j = 0; j < Size; j++)
        {
            if (j <= i)
                pMatrix[i * Size + j] = rand() / double(1000000);
            else
                pMatrix[i * Size + j] = 0;
        }
    }
}

// Function for formatted matrix output
void PrintMatrix(double *pMatrix, int RowCount, int ColCount)
{
    for (int i = 0; i < RowCount; i++)
    {
        for (int j = 0; j < ColCount; j++)
            printf("%9.4f ", pMatrix[i * ColCount + j]);
        printf("\n");
    }
}

// Function for formatted vector output
void PrintVector(double *pVector, int Size)
{
    for (int i = 0; i < Size; i++)
        printf("%7.4f ", pVector[i]);
    printf("\n");
}

// Function for memory allocation and data initialization
void ProcessInitialization(double *&pMatrix, double *&pVector, double *&pResult, int &Size)
{
    // Setting the size of the matrix and the vector
    do
    {
        printf("\nEnter the size of the matrix and the vector: ");
        scanf("%d", &Size);
        printf("\nChosen size = %d", Size);
        if (Size <= 0)
            printf("\nSize of objects must be greater than 0!\n");
    } while (Size <= 0);

    // Memory allocation
    pMatrix = new double[Size * Size];
    pVector = new double[Size];
    pResult = new double[Size];

    RandomDataInitialization(pMatrix, pVector, Size);
}

// Function for computational process termination
void ProcessTermination(double *pMatrix, double *pVector, double *pResult)
{
    delete[] pMatrix;
    delete[] pVector;
    delete[] pResult;
}

int main()
{
    double *pMatrix; // Matrix of the linear system
    double *pVector; // Right parts of the linear system
    double *pResult; // Result vector
    int Size;        // Size of the matrix and the vector
    time_t start, finish;
    double duration;

    printf("Serial Gauss algorithm for solving linear systems\n");
    ProcessInitialization(pMatrix, pVector, pResult, Size);
    // Matrix and vector output
    printf("\nInitial Matrix \n");
    PrintMatrix(pMatrix, Size, Size);
    printf("\nInitial Vector \n");
    PrintVector(pVector, Size);

    // Process termination
    ProcessTermination(pMatrix, pVector, pResult);
}