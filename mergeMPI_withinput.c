#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define FILENAME "smallreversed.txt"

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

void mergeSort(int arr[], int n) {
    if (n > 1) {
        int mid = n / 2;
        int *left = (int *)malloc(mid * sizeof(int));
        int *right = (int *)malloc((n - mid) * sizeof(int));

        for (int i = 0; i < mid; i++) {
            left[i] = arr[i];
        }
        for (int i = mid; i < n; i++) {
            right[i - mid] = arr[i];
        }

        mergeSort(left, mid);
        mergeSort(right, n - mid);

        merge(arr, left, right, mid, n - mid);

        free(left);
        free(right);
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

     

       printf("Original array: ");
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

    mergeSort(localArray, localSize);

    int *sortedArray = NULL;

    if (rank == 0) {
        sortedArray = (int *)malloc(sizeof(int) * arraySize);
    }

    MPI_Gatherv(localArray, localSize, MPI_INT, sortedArray, recvCounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        mergeSort(sortedArray, arraySize);

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
