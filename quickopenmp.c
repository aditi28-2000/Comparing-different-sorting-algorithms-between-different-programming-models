#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] <= pivot) {
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

        #pragma omp parallel sections
        {
            #pragma omp section
            {
                // Sort elements before the partition
                quickSort(arr, low, pi - 1);
            }

            #pragma omp section
            {
                // Sort elements after the partition
                quickSort(arr, pi + 1, high);
            }
        }
    }
}

void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int main() {
    FILE *file = fopen("largereversed.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening the file.\n");
        return 1;
    }

    int value, capacity = 20, size = 0;
    int *arr = (int *)malloc(capacity * sizeof(int));

    while (fscanf(file, "%d,", &value) == 1) {
        if (size == capacity) {
            capacity *= 2;
            arr = (int *)realloc(arr, capacity * sizeof(int));
        }
        arr[size++] = value;
    }

    fclose(file);
    printf("Original array (first 20 elements): \n");
    printArray(arr, (size < 20) ? size : 20);

    // Parallel sorting of the entire array
    #pragma omp parallel
    {
        #pragma omp single
        {
            quickSort(arr, 0, size - 1);
        }
    }

    printf("Sorting %d numbers\n", size);
    printf("Sorted array (first 20 elements): \n");
    printArray(arr, (size < 20) ? size : 20);

    free(arr);

    return 0;
}
