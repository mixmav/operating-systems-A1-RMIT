#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sched.h>

#define PHILOSOPHERS_COUNT 5

bool is_running = true;
pthread_mutex_t forks[PHILOSOPHERS_COUNT];

//Declare prototype
void *philosopher_thread(void *ptr);

//Custom struct to aid naming
struct Thread{
	pthread_t phil;
	int index;
};

int main(){
	struct Thread philosophers[PHILOSOPHERS_COUNT];

	int mutexInitCounter = 0;
	while(mutexInitCounter < PHILOSOPHERS_COUNT){
		pthread_mutex_init(&forks[mutexInitCounter], NULL);
		mutexInitCounter++;
	}

	int pthreadCreateCounter = 0;
	while (pthreadCreateCounter < PHILOSOPHERS_COUNT){
		philosophers[pthreadCreateCounter].index = pthreadCreateCounter;
		pthread_create(&philosophers[pthreadCreateCounter].phil, NULL, philosopher_thread, &philosophers[pthreadCreateCounter]);
		pthreadCreateCounter++;
	}

	int sleepTime = 10;
	sleep(sleepTime);

	//Exit global loop
	is_running = false;

	int pthreadJoinCounter = 0;
	while(pthreadJoinCounter < PHILOSOPHERS_COUNT){
		pthread_join(philosophers[pthreadJoinCounter].phil, NULL);
		pthreadJoinCounter++;
	}

	int mutexDestroyCounter = 0;
	while(mutexDestroyCounter < PHILOSOPHERS_COUNT){
		pthread_mutex_destroy(&forks[mutexDestroyCounter]);
		mutexDestroyCounter++;
	}

	return 0;
}

void *philosopher_thread(void *ptr)
{
	struct Thread *phil_thread = (struct Thread *)ptr;
	while (is_running){
		printf("Philosopher %d is thinking right now \n", phil_thread->index);

		sched_yield();

		int randomMiliseconds1 = (rand() % 1000)*1000;
		usleep(randomMiliseconds1);

		// Flag to check fork availability
		bool fork_available = false;

		while(!fork_available) {
			bool pThreadMutexTry1 = pthread_mutex_trylock(&forks[phil_thread->index]);
			switch(pThreadMutexTry1){
				case true:
					pthread_mutex_unlock(&forks[phil_thread->index]);
					sched_yield();
					usleep((rand() % 1000) * 1000);
					continue;
					break;
			}

			bool pThreadMutexTry2 = pthread_mutex_trylock(&forks[(phil_thread->index + PHILOSOPHERS_COUNT - 1) % PHILOSOPHERS_COUNT]);
			switch(pThreadMutexTry2){
				case true:
					pthread_mutex_unlock(&forks[(phil_thread->index + PHILOSOPHERS_COUNT - 1) % PHILOSOPHERS_COUNT]);
					sched_yield();
					usleep((rand() % 1000) * 1000);
					continue;
					break;
			}

			fork_available = true;
		}


		printf("Philosopher %d is eating right now \n", phil_thread->index);

		sched_yield();
		int randomMiliseconds2 = (rand() % 1000) * 1000;
		usleep(randomMiliseconds2);

		pthread_mutex_unlock(&forks[phil_thread->index]);
		pthread_mutex_unlock(&forks[(phil_thread->index + PHILOSOPHERS_COUNT - 1) % PHILOSOPHERS_COUNT]);

		sched_yield();
	}


	return NULL;
}