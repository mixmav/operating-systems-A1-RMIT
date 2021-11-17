#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>

#define total_items 5
#define buffer_size 10

int in = 0;
int out = 0;
int buffer[buffer_size];

sem_t empty_semaphore;
sem_t full_semaphore;

pthread_mutex_t mutex;
bool is_running = true;

void *producer(void *pno) {
	int item;

	while(is_running) {
		int producerCounter = 0;

		while(producerCounter < total_items) {
			// random item till range 1000
			item = rand() % 1000;

			sem_wait(&empty_semaphore);

			//Lock the buffer
			pthread_mutex_lock(&mutex);

			buffer[in] = item;

			printf("Item %d is being added by producer %d at position %d\n", buffer[in], *((int *)pno), in);

			in = (in+1) % buffer_size;

			//Unlock the buffer
			pthread_mutex_unlock(&mutex);
			sem_post(&full_semaphore);
			producerCounter++;
		}
	}
	
	return NULL;
}

void *consumer(void *cno) {
	while(is_running) {
		int consumerCounter = 0;
		while(consumerCounter < total_items) {
			sem_wait(&full_semaphore);

			//Lock the buffer
			pthread_mutex_lock(&mutex);
			
			int item = buffer[out];
			printf("Item %d: is being removed by consumer %d from position %d\n", item, *((int *)cno), out);
			out = (out+1) % buffer_size;

			//Unlock the buffer
			pthread_mutex_unlock(&mutex); 

			sem_post(&empty_semaphore);
			consumerCounter++;
		}
	}
	return NULL;
}

int main()
{
	pthread_t pro_arr[5], con_arr[5];
	pthread_mutex_init(&mutex, NULL);

	sem_init(&empty_semaphore, 0, buffer_size);
	sem_init(&full_semaphore, 0, 0);

	int name[5] = { 1, 2, 3, 4, 5 };
	
	int producerCreatorCounter = 0;
	while(producerCreatorCounter < 5) {

		//Create producer thread
		pthread_create(&pro_arr[producerCreatorCounter], NULL, producer, (void *)&name[producerCreatorCounter]);
		producerCreatorCounter++;
	}

	int consumerCreatorCounter = 0;
	while(consumerCreatorCounter < 5) {

		//Create consumer thread
		pthread_create(&con_arr[consumerCreatorCounter], NULL, consumer, (void *)&name[consumerCreatorCounter]);
		consumerCreatorCounter++;
	}

	int sleepTime = 10;
	sleep(sleepTime);

	// exit the global loop
	is_running = false;

	int threadJoinCounter1 = 0;
	while(threadJoinCounter1 < 5){
		// Terminate the producer thread
		pthread_join(pro_arr[threadJoinCounter1], NULL);
		threadJoinCounter1++;
	}

	int threadJoinCounter2 = 0;
	while(threadJoinCounter2 < 5){
		//Terminate the consumer thread
		pthread_join(con_arr[threadJoinCounter2], NULL);
		threadJoinCounter2++;
	}

	//Cleanup to stop memory leaks
	sem_destroy(&full_semaphore);
	sem_destroy(&empty_semaphore);
	pthread_mutex_destroy(&mutex);
	
	return 0;
}