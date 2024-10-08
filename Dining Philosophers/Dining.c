#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

void* philosopher(void* i);
void take_forks(int i);
void put_forks(int i);
void test(int i);
void think(int i);
void eat(int i);
void check_hunger_threshold(int i);
void* print_stuff(void* nuthin);
const char* state_to_string(int state);

#define LEFT ((int)i + 5 - 1) % 5
#define RIGHT ((int)i + 1) % 5
#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define HUNGER_THRESHOLD 100

int running = 1;

pthread_t philoph_thread[5];
int state[5];
int hunger[5] = {0};
pthread_mutex_t mp;
sem_t forks[5];

int main(void) {
    pthread_t print_thread;

    srand(time(NULL));

    for(int i = 0; i < 5; i++) {
        hunger[i] = rand() % 95;
        printf("  Philospher %d starting hunger: %d\n", i, hunger[i]);
    }
    printf("\n");

    pthread_mutex_init(&mp, NULL);
    for(int i = 0; i < 5; i++) {
        sem_init(&forks[i], 0, 0); // initialize to 0 so philosopher must wait for forks to be available
    }

    for (long i = 0; i < 5; i++) {
        pthread_create(&philoph_thread[i], NULL, philosopher, (void*)i);
    }
    pthread_create(&print_thread, NULL, print_stuff, NULL);

    for (int i = 0; i < 5; i++) {
        pthread_join(philoph_thread[i], NULL);
    }
    pthread_join(print_thread, NULL);

    pthread_mutex_destroy(&mp);
    for(int i = 0; i < 5; i++) {
        sem_destroy(&forks[i]);
    }

    printf("\nSTARVED\n\n");
    return 0;
}

void* philosopher(void* i) {
    int j = (int)(long)i;

    while(running) {
        think(j);
        take_forks(j);
        eat(j);
        put_forks(j);
    }

    return NULL;
}

void take_forks(int i) {
    pthread_mutex_lock(&mp);
    state[i] = HUNGRY;
    test(i);
    pthread_mutex_unlock(&mp);
    sem_wait(&forks[i]);
}

void put_forks(int i) {
    pthread_mutex_lock(&mp);
    state[i] = THINKING;
    test(LEFT); // test both neighbors to allow them to eat if the the other fork is also available and the neighbor is hungry
    test(RIGHT);
    pthread_mutex_unlock(&mp);
}

void test(int i) {
    // check if both neighbors are not eating, if not then both forks are available
    if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        state[i] = EATING;
        sem_post(&forks[i]); // up fork semaphore to indicate fork is available for use
    }
}

void check_hunger_threshold(int i) {
    if(hunger[i] >= HUNGER_THRESHOLD) {
        running = 0;
    }
}

void think(int i) {
    pthread_mutex_lock(&mp);
    hunger[i] += 5;
    check_hunger_threshold(i);
    pthread_mutex_unlock(&mp);
    sleep(rand() % 3 + 1); // Sleep for 1 to 3 seconds
}

void eat(int i) {
    pthread_mutex_lock(&mp);
    hunger[i] -= 5;
    pthread_mutex_unlock(&mp);
    sleep(rand() % 3 + 1); // Sleep for 1 to 3 seconds
}

const char* state_to_string(int state) {
    switch (state) {
        case THINKING: return "THINKING";
        case HUNGRY: return "HUNGRY";
        case EATING: return "EATING";
        default: return "UNKNOWN";
    }
}

void* print_stuff(void* nothin) {
    printf("----------------------------------------------------------------------------------------------------");
    while(running) {
        pthread_mutex_lock(&mp);

        printf("\n\t\t            Philospher %d, State: %s, Hunger: %d\n\n", 0, state_to_string(state[0]), hunger[0]);
        printf("  Philospher %d, State: %s, Hunger: %d\t\tPhilosopher %d, State: %s, Hunger: %d\n\n", 1, state_to_string(state[1]),  hunger[1], 2, state_to_string(state[2]), hunger[2]);
        printf("  Philospher %d, State: %s, Hunger: %d\t\tPhilosopher %d, State: %s, Hunger: %d\n\n", 3, state_to_string(state[3]),  hunger[3], 4, state_to_string(state[4]), hunger[4]);
        
        pthread_mutex_unlock(&mp);
        
        printf("----------------------------------------------------------------------------------------------------\n");

        sleep(2);
    }

    return NULL;
}