#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <stdbool.h>

#define SIZE 100000 // Using minimum requirement for faster testing

// --- Utility Functions ---
void generateArray(int arr[], int n) {
    for (int i = 0; i < n; i++) arr[i] = rand() % 100000;
}

bool isSorted(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        if (arr[i - 1] > arr[i]) return false;
    }
    return true;
}

void merge(int arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L);
    free(R);
}

void mergeSortSeq(int arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortSeq(arr, left, mid);
        mergeSortSeq(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

// --- Main MPI Program ---
int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Part 4 Requirement: Print Processor Name
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    printf("Process %d out of %d running on %s\n", rank, size, processor_name);

    int chunk_size = SIZE / size;
    int *local_arr = (int *)malloc(chunk_size * sizeof(int));
    int *global_arr = NULL;

    if (rank == 0) {
        global_arr = (int *)malloc(SIZE * sizeof(int));
        generateArray(global_arr, SIZE);
        printf("\nGenerating array of %d elements...\n", SIZE);
    }

    // Start timer on Process 0
    double start_time;
    if (rank == 0) start_time = MPI_Wtime();

    // 1. Scatter the array to all processes
    MPI_Scatter(global_arr, chunk_size, MPI_INT, local_arr, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // 2. Each process sorts its own chunk
    mergeSortSeq(local_arr, 0, chunk_size - 1);

    // 3. Gather the sorted chunks back to Process 0
    MPI_Gather(local_arr, chunk_size, MPI_INT, global_arr, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // 4. Process 0 merges the final gathered chunks together
    if (rank == 0) {
        int *final_arr = (int *)malloc(SIZE * sizeof(int));
        for (int i = 0; i < chunk_size; i++) final_arr[i] = global_arr[i];

        int current_size = chunk_size;
        for (int i = 1; i < size; i++) {
            for (int j = 0; j < chunk_size; j++) {
                final_arr[current_size + j] = global_arr[i * chunk_size + j];
            }
            merge(final_arr, 0, current_size - 1, current_size + chunk_size - 1);
            current_size += chunk_size;
        }

        double end_time = MPI_Wtime();
        printf("MPI Execution Time: %f seconds\n", end_time - start_time);
        printf("Verification: %s\n\n", isSorted(final_arr, SIZE) ? "PASSED" : "FAILED");
        free(final_arr);
    }

    if (rank == 0) free(global_arr);
    free(local_arr);

    MPI_Finalize();
    return 0;
}
