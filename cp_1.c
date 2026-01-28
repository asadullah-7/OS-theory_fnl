#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define TOTAL_PASSENGERS 9   // must be multiple of 3

// ===== GLOBAL VARIABLES =====
pthread_mutex_t lock;

sem_t boarding;      // controls available seats
sem_t allSeated;     // signals ride to start
sem_t rideOver;      // signals passengers to leave

int count = 0;       // passengers currently seated

// ===== FUNCTION DECLARATIONS =====
void* passanger_thread(void* arg);
void board_ride();
void start_ride();
void end_ride();

// ===== PASSENGER THREAD =====
void* passanger_thread(void* arg)
{
    int id = *(int*)arg;

    printf("Passenger %d has arrived and waiting to board\n", id);

    sem_wait(&boarding);   // wait for seat

    pthread_mutex_lock(&lock);
    count++;
    printf("Passenger %d boarded (count = %d)\n", id, count);

    if (count == 3)
    {
        sem_post(&allSeated);   // last passenger wakes ride
    }
    pthread_mutex_unlock(&lock);

    sem_wait(&rideOver);   // wait till ride finishes

    pthread_mutex_lock(&lock);
    count--;
    pthread_mutex_unlock(&lock);

    sem_post(&boarding);   // seat freed

    return NULL;
}

// ===== RIDE CONTROLLER =====
void board_ride()
{
    while (1)
    {
        sem_wait(&allSeated);   // wait until 3 passengers ready

        printf("\nAll three seats are filled\n");
        start_ride();
        end_ride();

        // release all 3 passengers
        sem_post(&rideOver);
        sem_post(&rideOver);
        sem_post(&rideOver);
    }
}

// ===== RIDE ACTIONS =====
void start_ride()
{
    printf("Ride started 🚀\n");
    sleep(2);
}

void end_ride()
{
    printf("Ride finished 🎢\n\n");
}

// ===== MAIN FUNCTION =====
int main()
{
    pthread_t passengers[TOTAL_PASSENGERS];
    pthread_t rideThread;
    int ids[TOTAL_PASSENGERS];

    // init mutex & semaphores
    pthread_mutex_init(&lock, NULL);

    sem_init(&boarding, 0, 3);     // 3 seats
    sem_init(&allSeated, 0, 0);
    sem_init(&rideOver, 0, 0);

    // start ride controller thread
    pthread_create(&rideThread, NULL, (void*)board_ride, NULL);

    // create passenger threads
    for (int i = 0; i < TOTAL_PASSENGERS; i++)
    {
        ids[i] = i + 1;
        pthread_create(&passengers[i], NULL, passanger_thread, &ids[i]);
        sleep(1);   // stagger arrivals
    }

    // wait for passengers
    for (int i = 0; i < TOTAL_PASSENGERS; i++)
    {
        pthread_join(passengers[i], NULL);
    }

    return 0;
}
