#include <iostream>
#include <iomanip>

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

// Function for memory allocation and data init
void processInitialization(double *&pMatrix, double *&pVector, double *&pResult, int &size)
{
    // Size for initial matrix and vectors
    do
    {
        std::cout << "\nEnter the size of the initial objects: ";
        std::cin >> size;
        if (size <= 0)
            std::cout << "\nThe size value must be positive number";
    } while (size <= 0);
    std::cout << "\nChosen object size: " << size << std::endl;

    pMatrix = new double[size * size];
    pVector = new double[size];
    pResult = new double[size];
    dummyDataInit(pMatrix, pVector, size);
}

void printMatrix(double *matrix, int rowCount, int colCount)
{
    for (int i = 0; i < rowCount; i++)
    {
        for (int j = 0; j < colCount; j++)
        {
            std::cout << std::setw(7) << std::fixed << std::setprecision(4) << matrix[i * colCount + j];
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}
void printVector(double *vector, int size)
{
    for (int i = 0; i < size; i++)
    {
        std::cout << std::setw(8) << std::fixed << std::setprecision(4) << vector[i];
    }
    std::cout << "\n";
}

// Function for matrix-vector multiplication
void ResultCalculation(double *pMatrix, double *pVector, double *pResult, int size)
{
    for (int i = 0; i < size; i++)
    {
        pResult[i] = 0;
        for (int j = 0; j < size; j++)
            pResult[i] += pMatrix[i * size + j] * pVector[j];
    }
}

void ProcessTermination(double *pMatrix, double *pVector, double *pResult)
{
    delete[] pMatrix;
    delete[] pVector;
    delete[] pResult;
}

int main()
{
    double *pMatrix;
    double *pVector;
    double *pResult;
    int size;

    processInitialization(pMatrix, pVector, pResult, size);
    std::cout << "\nInitial matrix:\n";
    printMatrix(pMatrix, size, size);
    std::cout << "Initial vector:\n";
    printVector(pVector, size);

    // Matrix-vector multiplication
    ResultCalculation(pMatrix, pVector, pResult, size);
    // Result vector
    printVector(pResult, size);

    // Computational process termination
    ProcessTermination(pMatrix, pVector, pResult);

    return 0;
}