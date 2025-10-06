#include <iostream>

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
}

int main()
{
    double *pMatrix;
    double *pVector;
    double *pResult;
    int size;

    processInitialization(pMatrix, pVector, pResult, size);
    std::cout << "Hello world";
    return 0;
}