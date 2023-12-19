#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void merge(int arr[], int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    // Create temporary arrays
    int L[n1], R[n2];

    // Copy data to temporary arrays L[] and R[]
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    // Merge the temporary arrays back into arr[l..r]
    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of L[], if there are any
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Copy the remaining elements of R[], if there are any
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(int arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;

        // Sort first and second halves
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);

        // Merge the sorted halves
        merge(arr, l, m, r);
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
    mergeSort(arr, 0, size - 1);
    printf("Sorting %d numbers\n",size);
    printf("Sorted array (first 20 elements): \n");
    printArray(arr, (size < 20) ? size : 20);

    free(arr); // Free dynamically allocated memory

    return 0;
}
