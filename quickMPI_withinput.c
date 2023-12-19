#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define FILENAME "largenearlysorted.txt"

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }

    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

void quickSort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);

        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
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
        FILE *file = fopen(FILENAME, "r");
        if (file == NULL) {
            fprintf(stderr, "Error opening file %s\n", FILENAME);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        arraySize = 0;
        int num;
        while (fscanf(file, "%d, ", &num) == 1) {
            arraySize++;
        }

        fclose(file);

        array = (int *)malloc(sizeof(int) * arraySize);
        if (array == NULL) {
            fprintf(stderr, "Error allocating memory for the array\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        file = fopen(FILENAME, "r");
        if (file == NULL) {
            fprintf(stderr, "Error opening file %s\n", FILENAME);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        for (int i = 0; i < arraySize; i++) {
            if (fscanf(file, "%d, ", &array[i]) != 1) {
                fprintf(stderr, "Error reading from file %s\n", FILENAME);
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }

        printf("Original array:\n");
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

    MPI_Scatterv(array, recvCounts, displs, MPI_INT, localArray, localSize, MPI_INT, 0, MPI_COMM_WORLD);

    quickSort(localArray, 0, localSize - 1);

    int *sortedArray = NULL;

    if (rank == 0) {
        sortedArray = (int *)malloc(sizeof(int) * arraySize);
    }

    MPI_Gatherv(localArray, localSize, MPI_INT, sortedArray, recvCounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        quickSort(sortedArray, 0, arraySize - 1);

        /*printf("First 20 elements of Original Array: ");
        for (int i = 0; i < 20; i++) {
            printf("%d ", array[i]);
        }
        printf("\n");*/

        printf("First 20 elements of Sorted Array: ");
        for (int i = 0; i < 20; i++) {
            printf("%d ", sortedArray[i]);
        }
        printf("\n");

        free(array);
        free(sortedArray);
    }

    free(recvCounts);
    free(displs);
    free(localArray);

    MPI_Finalize();
    return 0;
}
