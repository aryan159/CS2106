#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>

#define NUM_PROCESSES 5

int main()
{

    int i, j, pid;

    sem_t *semArr;

    int shmid = shmget(IPC_PRIVATE, sizeof(sem_t) * NUM_PROCESSES, IPC_CREAT | 0600);
    semArr = (sem_t *)shmat(shmid, NULL, 0);

    for (int k = 0; k < NUM_PROCESSES; k++)
    {
        sem_init(&semArr[k], 1, 0);
    }

    sem_post(&semArr[0]);

    for (i = 0; i < NUM_PROCESSES; i++)
    {
        if ((pid = fork()) == 0)
        {
            break;
        }
    }

    if (pid == 0)
    {
        sem_wait(&semArr[i]);

        printf("I am child %d\n", i);

        for (j = i * 10; j < i * 10 + 10; j++)
        {
            printf("%d ", j);
            fflush(stdout);
            usleep(250000);
        }

        printf("\n\n");

        if (i < (NUM_PROCESSES - 1))
        {
            sem_post(&semArr[i + 1]);
        }
    }
    else
    {
        for (i = 0; i < NUM_PROCESSES; i++)
            wait(NULL);
    }
}
