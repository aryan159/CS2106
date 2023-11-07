#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>

int nproc = 0, count = 0;
int shmidBarrier, shmidCount, shmidData;
sem_t *barrierSem, *countSem;
int *sharedData;

void init_barrier(int numproc)
{
    nproc = numproc;

    shmidBarrier = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0600);
    shmidCount = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0600);
    shmidData = shmget(IPC_PRIVATE, sizeof(int) * 2, IPC_CREAT | 0600);

    barrierSem = (sem_t *)shmat(shmidBarrier, NULL, 0);
    countSem = (sem_t *)shmat(shmidCount, NULL, 0);
    sharedData = (int *)shmat(shmidData, NULL, 0);

    sem_init(barrierSem, 1, 0);
    sem_init(countSem, 1, 0);
    sem_post(countSem);

    sharedData[0] = numproc; // equivalent to nproc
    sharedData[1] = 0;       // equivalent to count
}

void reach_barrier()
{
    // printf("here");

    sem_wait(countSem);
    sharedData[1]++;
    sem_post(countSem);

    // printf("%d %d", count, nproc);

    if (sharedData[1] == sharedData[0])
    {
        sem_post(barrierSem);
    }
    else
    {
        sem_wait(barrierSem);

        // DO WORK

        sem_post(barrierSem);
    }
}

void destroy_barrier(int my_pid)
{
    if (my_pid != 0)
    {
        // Destroy the semaphores and detach
        // and free any shared memory. Notice
        // that we explicity check that it is
        // the parent doing it.

        sem_destroy(countSem);
        sem_destroy(barrierSem);

        shmctl(shmidBarrier, IPC_RMID, 0);
        shmctl(shmidCount, IPC_RMID, 0);
    }
}
