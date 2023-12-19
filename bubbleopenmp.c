#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void bubbleSort(int arr[], int n) {
    int i, j;
    for (i = 0; i < n - 1; i++) {
        #pragma omp parallel for shared(arr) private(j)
        for (j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                // Swap arr[j] and arr[j+1]
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main() {
    FILE *file;
    
    // Open the file for reading
    file = fopen("largerandom.txt", "r");
    
    // Check if the file opened successfully
    if (file == NULL) {
        perror("Error opening file");
        return 1; // Return an error code
    }

    // Determine the number of integers in the file
    int count = 0;
    int number;
    while (fscanf(file, "%d,", &number) == 1) {
        count++;
    }

    // Reset the file position to the beginning
    fseek(file, 0, SEEK_SET);

    // Dynamically allocate memory for the array
    int *arr = malloc(count * sizeof(int));

    // Read integers from the file into the array
    for (int i = 0; i < count; i++) {
        if (fscanf(file, "%d,", &arr[i]) != 1) {
            perror("Error reading from file");
            free(arr);
            fclose(file);
            return 1; // Return an error code
        }
    }
    printf("Origial Array (first 20 elements): ");
    for (int i = 0; i < 20 && i < count; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    printf("Sorting %d numbers \n",count);    
// Close the file
    fclose(file);

    // Sort the array using Bubble Sort with OpenMP
    bubbleSort(arr, count);

    printf("Sorted Array (first 20 elements): ");
    for (int i = 0; i < 20 && i < count; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    // Free the dynamically allocated memory
    free(arr);

    return 0;
}
