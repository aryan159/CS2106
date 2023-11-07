#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include "config.h"
#include "barrier.h"
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>

#define NUM_PROCESSES 8

int main()
{

    int vect[VECT_SIZE];
    int pid;

    int shmidLargest = shmget(IPC_PRIVATE, sizeof(int) * NUM_PROCESSES, IPC_CREAT | 0600);
    int shmidSmallest = shmget(IPC_PRIVATE, sizeof(int) * NUM_PROCESSES, IPC_CREAT | 0600);

    int *largest = (int *)shmat(shmidLargest, NULL, 0);
    int *smallest = (int *)shmat(shmidSmallest, NULL, 0);

    // int largest[NUM_PROCESSES], smallest[NUM_PROCESSES];

    init_barrier(NUM_PROCESSES + 1);

    float per_process_raw = (float)VECT_SIZE / NUM_PROCESSES;
    int per_process = (int)per_process_raw;

    clock_t start, end;
    double time_taken;

    if (per_process_raw != (float)per_process)
    {
        printf("Vector size of %d is not divisible by %d processes.\n", VECT_SIZE, NUM_PROCESSES);
        exit(-1);
    }

    srand(24601);
    int i;

    for (i = 0; i < VECT_SIZE; i++)
    {
        vect[i] = rand();
    }

    for (i = 0; i < NUM_PROCESSES; i++)
    {
        pid = fork();

        if (pid == 0)
            break;
    }

    int j;
    int big = -INT_MAX;
    int small = INT_MAX;

    if (pid == 0)
    {
        int start = i * per_process;
        int end = i * per_process + per_process;

        for (j = start; j < end; j++)
        {
            if (vect[j] > big)
                big = vect[j];

            if (vect[j] < small)
                small = vect[j];
        }
        largest[i] = big;
        smallest[i] = small;

        reach_barrier();
    }
    else
    {
        reach_barrier();
        start = clock();
        for (j = 0; j < NUM_PROCESSES; j++)
        {
            if (largest[j] > big)
                big = largest[j];

            if (smallest[j] < small)
                small = smallest[j];
        }
        end = clock();
        time_taken = ((double)end - start) / CLOCKS_PER_SEC;

        printf("\nNumber of items: %d\n", VECT_SIZE);
        printf("Smallest element is %d\n", small);
        printf("Largest element is %d\n", big);
        printf("Time taken is %3.2fs\n\n", time_taken);

        // Clean up process table
        for (j = 0; j < NUM_PROCESSES; j++)
            wait(NULL);
    }
}
