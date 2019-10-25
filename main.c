#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "debug.h"
#include "memory.h"


#include "args.h"

#define C_ERRO_PTHREAD_CREATE           1
#define C_ERRO_PTHREAD_JOIN             2
#define C_ERRO_MUTEX_INIT               3
#define C_ERRO_MUTEX_DESTROY            4
#define C_ERRO_CONDITION_INIT           5
#define C_ERRO_CONDITION_DESTROY        6

#define MAX 		5 		/* Capacidade do buffer */
#define LIMITE 		20 		/* Total de elementos a produzir */

#define MAX_WEIGHT  4 // in ton's
#define TIME_PASSING 3 //in seconds

typedef struct
{
	int totalWeight;
	int passingTimes;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
} shared_t;

typedef struct
{	
	char name[3];
	int weight;
	int timesPassed;
	shared_t *shared;
} vehicle_info;

void *passing(void *arg);


/* main */
int main(int argc, char *argv[]) 
{
	struct gengetopt_args_info args_info;

	if(cmdline_parser(argc, argv, &args_info) != 0) {
		printf("ERROR");
		return 1;
	}

	int numVehicles = args_info.vehicles_arg;
	int numPassages = args_info.passages_arg;

	srand(time(NULL));   // Initialization, should only be called once.
	
	//vehicle_info *vehicle_info = MALLOC(sizeof(vehicle_info) * numVehicles);
	vehicle_info vehicle_info[4];
	pthread_t *vehicle_threads = MALLOC(sizeof(pthread_t) * numVehicles);
	shared_t shared;
	pthread_mutex_t mutex;
	pthread_cond_t cond;

	for (int i = 0; i < numVehicles; i++) {
		vehicle_info[i].weight = (rand() % 3) + 1;
		vehicle_info[i].timesPassed = 0;
		sprintf(vehicle_info[i].name, "V%d", (i+1));
		printf("Size of name: %ld\n", strlen(vehicle_info[i].name));
		printf("Weight %s: %d\n", vehicle_info[i].name, vehicle_info[i].weight);
	}
	printf("----------------\n");
	
    //Inicia o mutex 
	if ((errno = pthread_mutex_init(&mutex, NULL)) != 0)
		ERROR(C_ERRO_MUTEX_INIT, "pthread_mutex_init() failed!");

    // Inicia variavel de condicao
	if ((errno = pthread_cond_init(&cond, NULL)) != 0)
		ERROR(C_ERRO_CONDITION_INIT, "pthread_cond_init() failed!");

	shared.mutex = mutex;
	shared.cond = cond;
	shared.totalWeight = 0;
	shared.passingTimes = numPassages;

    for(int i = 0; i < numVehicles; i++) {
		vehicle_info[i].shared = &shared;
	}
	
	for(int i = 0; i < numVehicles; i++) {
		if ((errno = pthread_create(&(vehicle_threads[i]), NULL, passing, &vehicle_info[i])) != 0)
			ERROR(C_ERRO_PTHREAD_CREATE, "pthread_create() failed!");
	}
	
	for(int i = 0; i < numVehicles; i++) {
		if ((errno = pthread_join(vehicle_threads[i], NULL)) != 0)
		ERROR(C_ERRO_PTHREAD_JOIN, "pthread_join() failed!");
	}
	
	if ((errno = pthread_mutex_destroy(&mutex)) != 0)
		ERROR(C_ERRO_MUTEX_DESTROY, "pthread_mutex_destroy() failed!");
	
	if ((errno = pthread_cond_destroy(&cond)) != 0)
		ERROR(C_ERRO_CONDITION_DESTROY, "pthread_cond_destroy() failed!");
	
    return 0;
}

void *passing(void *args) {
	vehicle_info *param = (vehicle_info *) args;
	printf("%s %p\n", param->name, param);
	//printf("Weight: %d || Vehicle: %s\n", param->shared->totalWeight, param->name);
	/*
	do{
		
		if ((errno = pthread_mutex_lock(&(param->shared->mutex))) != 0) {
			WARNING("pthread_mutex_lock() failed\n");
			return NULL;
		}

		while (param->shared->totalWeight >= MAX_WEIGHT){
			if ((errno = pthread_cond_wait(&(param->shared->cond), &(param->shared->mutex))) != 0) {
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
		}

		param->shared->totalWeight += param->weight;
		printf("%s entered the bridge (total weight: %d)\n", param->name, param->shared->totalWeight);

		 //Notifica consumidores  espera 
        if (param->shared->totalWeight < MAX_WEIGHT) {
			if ((errno = pthread_cond_signal(&(param->shared->cond))) != 0) {
				WARNING("pthread_cond_signal() failed");
				return NULL;
			}
		}

		param->shared->totalWeight -= param->weight;

        //Sai da seccao critica
		if ((errno = pthread_mutex_unlock(&(param->shared->mutex))) != 0) {
			WARNING("pthread_mutex_unlock() failed");
			return NULL;
		}

		printf("%s left the bridge (total weight: %d)\n", param->name, param->shared->totalWeight);
		param->timesPassed++;
	}while(param->timesPassed < param->shared->passingTimes);
	*/
	return NULL;
}
