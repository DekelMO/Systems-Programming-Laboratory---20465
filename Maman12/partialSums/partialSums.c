#include <stdio.h>
#include <stdlib.h>

/* Function to calculate partial sums */
int* calculatePartialSums(int size, int* arr) {
    int i;
    int temp_sum = 0;
    int* sum_array = (int*)malloc(size * sizeof(int));
    if (!sum_array) {
        printf("Memory allocation failed.\n");
        exit(1);
    } else {
        for (i = 0; i < size; ++i) {
            temp_sum += arr[i];
            sum_array[i] = temp_sum;
        }
    }
    return sum_array;
}

int main() {
    int size;
    int* arr;
    int* sum_array;
    int index;

    /* Prompt user for array size */
    printf("Hello, please enter the size of the array: ");
    scanf("%d", &size);

    /* Allocate memory for the original array */
    arr = (int*)malloc(size * sizeof(int));
    if (!arr) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    /* Prompt user to input numbers for the array */
    printf("Hello, please enter %d integers: ", size);
    for (index = 0; index < size; ++index) {
        scanf("%d", &arr[index]);
    }

    /* Print the input array */
    printf("input array: ");
    for (index = 0; index < size; ++index) {
        printf("%d ", arr[index]);
    }
    printf("\n");
    /* Calculate partial sums */
    sum_array = calculatePartialSums(size, arr);

    /* Print the resulting partial sums */
    printf("Partial Sums:");
    for (index = 0; index < size; ++index) {
        printf("%d ", sum_array[index]);
    }
    printf("\n");

    /* Clean up allocated memory */
    free(sum_array);
    free(arr);

    return 0;
}

