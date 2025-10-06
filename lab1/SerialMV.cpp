#include <iostream>

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

void printMatrix(double *matrix, int size)
{
    std::cout << "Matrix:\n";
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            std::cout << matrix[i * size + j];
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}
void printVector(double *vector, int size)
{
    std::cout << "Vector:\n";
    for (int i = 0; i < size; i++)
    {
        std::cout << vector[i];
    }
    std::cout << "\n";
}

int main()
{
    double *pMatrix;
    double *pVector;
    double *pResult;
    int size;

    processInitialization(pMatrix, pVector, pResult, size);
    printMatrix(pMatrix, size);
    printVector(pVector, size);
    return 0;
}