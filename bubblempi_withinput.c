#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define FILENAME "smallreversed.txt"

// Function to swap two integers
void swap(int *xp, int *yp) {
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// Function to perform bubble sort
void bubbleSort(int arr[], int n) {
    int i, j;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
            }
        }
    }
}

// Function to merge two sorted arrays
void merge(int arr[], int left[], int right[], int leftSize, int rightSize) {
    int i = 0, j = 0, k = 0;
    while (i < leftSize && j < rightSize) {
        if (left[i] <= right[j]) {
            arr[k++] = left[i++];
        } else {
            arr[k++] = right[j++];
        }
    }
    while (i < leftSize) {
        arr[k++] = left[i++];
    }
    while (j < rightSize) {
        arr[k++] = right[j++];
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    int *array = NULL;
    int arraySize;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        // Read array size from file
        FILE *file = fopen(FILENAME, "r");
        if (file == NULL) {
            fprintf(stderr, "Error opening file %s\n", FILENAME);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        arraySize = 0;
        int num;
        // Adjusted format specifier to account for space before comma
        while (fscanf(file, "%d, ", &num) == 1) {
            arraySize++;
        }

        fclose(file);

        // Allocate memory for the array
        array = (int *)malloc(sizeof(int) * arraySize);
        if (array == NULL) {
            fprintf(stderr, "Error allocating memory for the array\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Read array from file
        file = fopen(FILENAME, "r");
        if (file == NULL) {
            fprintf(stderr, "Error opening file %s\n", FILENAME);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        for (int i = 0; i < arraySize; i++) {
            // Adjusted format specifier to account for space before comma
            if (fscanf(file, "%d, ", &array[i]) != 1) {
                fprintf(stderr, "Error reading from file %s\n", FILENAME);
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
       printf("First 20 elements of Original Array: ");
        for (int i = 0; i < 20; i++) {
            printf("%d ", array[i]);
        }
        printf("\n");

        fclose(file);
    }

    MPI_Bcast(&arraySize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int elementsPerProcess = arraySize / size;
    int remainder = arraySize % size;

    int *recvCounts = (int *)malloc(sizeof(int) * size);
    int *displs = (int *)malloc(sizeof(int) * size);

    for (int i = 0; i < size; ++i) {
        recvCounts[i] = elementsPerProcess + (i < remainder ? 1 : 0);
        displs[i] = (i > 0) ? displs[i - 1] + recvCounts[i - 1] : 0;
    }

    int localSize = recvCounts[rank];
    int *localArray = (int *)malloc(sizeof(int) * localSize);

    // Scatter portions of the array across processes
    MPI_Scatterv(array, recvCounts, displs, MPI_INT, localArray, localSize, MPI_INT, 0, MPI_COMM_WORLD);

    // Perform local bubble sort on each process
    bubbleSort(localArray, localSize);

    // Gather sorted arrays back to the root process
    MPI_Gatherv(localArray, localSize, MPI_INT, array, recvCounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        int tempArray[arraySize];

        merge(tempArray, array, array + elementsPerProcess, elementsPerProcess, arraySize - elementsPerProcess);

        // Sort the merged array
        bubbleSort(tempArray, arraySize);

        /*printf("First 20 elements of Original Array: ");
        for (int i = 0; i < 20; i++) {
            printf("%d ", array[i]);
        }
        printf("\n");*/

        printf("First 20 elements of Sorted Array: ");
        for (int i = 0; i < 20; i++) {
            printf("%d ", tempArray[i]);
        }
        printf("\n");

        // Free allocated memory
        free(array);
    }

    free(recvCounts);
    free(displs);
    free(localArray);

    MPI_Finalize();
    return 0;
}
