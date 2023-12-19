#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void bubbleSort(int arr[], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
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
    // Open the file for reading
    FILE *file = fopen("largerandom.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening the file.\n");
        return 1;
    }

    int value, capacity = 20, size = 0;
    int *arr = (int *)malloc(capacity * sizeof(int));

    // Read from the file into the dynamically allocated array
    while (fscanf(file, "%d,", &value) == 1) {
        if (size == capacity) {
            capacity *= 2; // Double the capacity if needed
            arr = (int *)realloc(arr, capacity * sizeof(int));
        }
        arr[size++] = value;
    }
    printf("Original array (first 20 elements): \n");
    printArray(arr, (size < 20) ? size : 20);
    fclose(file); // Close the file

    // Sorting the entire array serially
    bubbleSort(arr, size);
    printf("Sorting %d numbers\n",size);
    printf("Sorted array (first 20 elements): \n");
    printArray(arr, (size < 20) ? size : 20);

    free(arr); // Free dynamically allocated memory

    return 0;
}
