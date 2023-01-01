// ------------
// This code is provided solely for the personal and private use of
// students taking the CSC369H5 course at the University of Toronto.
// Copying for purposes other than this use is expressly prohibited.
// All forms of distribution of this code, whether as given or with
// any changes, are expressly prohibited.
//
// Authors: Bogdan Simion
//
// All of the files in this directory and all subdirectories are:
// Copyright (c) 2019 Bogdan Simion
// -------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "executor.h"

extern struct executor tassadar;


/**
 * Populate the job lists by parsing a file where each line has
 * the following structure:
 *
 * <id> <type> <num_resources> <resource_id_0> <resource_id_1> ...
 *
 * Each job is added to the queue that corresponds with its job type.
 */
void parse_jobs(char *file_name) {
    int id;
    struct job *cur_job;
    struct admission_queue *cur_queue;
    enum job_type jtype;
    int num_resources, i;
    FILE *f = fopen(file_name, "r");

    /* parse file */
    while (fscanf(f, "%d %d %d", &id, (int*) &jtype, (int*) &num_resources) == 3) {

        /* construct job */
        cur_job = malloc(sizeof(struct job));
        cur_job->id = id;
        cur_job->type = jtype;
        cur_job->num_resources = num_resources;
        cur_job->resources = malloc(num_resources * sizeof(int));

        int resource_id; 
				for(i = 0; i < num_resources; i++) {
				    fscanf(f, "%d ", &resource_id);
				    cur_job->resources[i] = resource_id;
				    tassadar.resource_utilization_check[resource_id]++;
				}
				
				assign_processor(cur_job);

        /* append new job to head of corresponding list */
        cur_queue = &tassadar.admission_queues[jtype];
        cur_job->next = cur_queue->pending_jobs;
        cur_queue->pending_jobs = cur_job;
        cur_queue->pending_admission++;
    }

    fclose(f);
}

/*
 * Magic algorithm to assign a processor to a job.
 */
void assign_processor(struct job* job) {
    int i, proc = job->resources[0];
    for(i = 1; i < job->num_resources; i++) {
        if(proc < job->resources[i]) {
            proc = job->resources[i];
        }
    }
    job->processor = proc % NUM_PROCESSORS;
}


void do_stuff(struct job *job) {
    /* Job prints its id, its type, and its assigned processor */
    printf("%d %d %d\n", job->id, job->type, job->processor);
}



/**
 * TODO: Fill in this function
 *
 * Do all of the work required to prepare the executor
 * before any jobs start coming
 * 
 */
void init_executor() {
	//Initialize all values for struct executor tassadar 
	
	//Initialize a mutex lock for each resource and resource_utilization_check
	for (int i = 0; i < NUM_RESOURCES; i++){
		pthread_mutex_init(&tassadar.resource_locks[i], NULL);
		tassadar.resource_utilization_check[i] = 0;
	}
	for (int i = 0; i < NUM_QUEUES; i++){
		//Initialize mutex lock for each admission queue
		pthread_mutex_init(&tassadar.admission_queues[i].lock, NULL);
		//Initialize condition variable for synchronization
		pthread_cond_init(&tassadar.admission_queues[i].admission_cv, NULL);
		pthread_cond_init(&tassadar.admission_queues[i].execution_cv, NULL);
		//Initializes the struct admission_queue's initial values 
		//& prepares to admit jobs once pending_jobs gets populated
		tassadar.admission_queues[i].head = 0;
		tassadar.admission_queues[i].tail = 0;
		tassadar.admission_queues[i].capacity = QUEUE_LENGTH;
		tassadar.admission_queues[i].num_admitted = 0;
		tassadar.admission_queues[i].admitted_jobs = malloc((sizeof (struct job*)) * QUEUE_LENGTH);
		tassadar.admission_queues[i].pending_admission = 0;
		tassadar.admission_queues[i].pending_jobs = NULL;
	}
	for(int i = 0; i < NUM_PROCESSORS; i++){
		//Initialize struct processor_record's initial values
		tassadar.processor_records[i].completed_jobs = NULL;
		tassadar.processor_records[i].num_completed = 0;
		pthread_mutex_init(&tassadar.processor_records[i].lock, NULL);
	}

}


/**
 * TODO: Fill in this function
 *
 * Handles an admission queue passed in through the arg (see the executor.c file). 
 * Bring jobs into this admission queue as room becomes available in it. 
 * As new jobs are added to this admission queue (and are therefore ready to be taken
 * for execution), the corresponding execute thread must become aware of this.
 * 
 */
void *admit_jobs(void *arg) {
    struct admission_queue *q = arg;
    //Note from Kannika: The structure of this entire function is followed by 
    //Condition Variables Lecture 4 Slide# 26 and from Bogdan's explanation

    //Keep looping until jobs are pending to get admitted into this queue
    while(q != NULL){
        pthread_mutex_lock(&(q->lock));
	//If no job is pendinding to get admission into this queue, then return
        if(q->pending_admission == 0){
	    	pthread_mutex_unlock(&(q->lock));
	    	return NULL;
        }

	//Wait until room becomes available in this admission_queue, only if the 
	//number of jobs in this admission_queue equals the maximum number of jobs it can hold
	while(q->capacity == 0){
	    //Wait until execution signals that this admission queue is not full
	    pthread_cond_wait(&(q->execution_cv), &(q->lock));
	}

	//Admit the job by transporting it from pending_jobs to admitted_jobs
	q->admitted_jobs[(q->tail)] = q->pending_jobs;
	
	//Update the value of the tail to point the new last element of admitted_jobs list 
        //(since it is a circular list implementation, mod is used to prevent list-out-of-index)
	q->tail = (q->tail + 1) % QUEUE_LENGTH;
	//Increment the number of elements in the admitted_jobs list since a job has just been admitted into this queue
	(q->num_admitted) += 1;
	//Set the pending_jobs to the next job waiting to get admission into this queue 
	//and decrement the capacity of this queue
	q->pending_jobs = (q->pending_jobs)->next;
	q->pending_admission -= 1;
	q->capacity -= 1;
	

	//Release lock and signal to admission_cv that this admission queue is not empty 
	//and let the corresponding execute thread be aware that a job is ready for execution
	pthread_cond_signal(&(q->admission_cv));
	pthread_mutex_unlock(&(q->lock));
	
    }

    return NULL;
}

int compare(const void * a, const void *b){
	int * aa = (int *) a;
	int * bb = (int *) b;
	return ((*aa)-(*bb));
}


/**
 * TODO: Fill in this function
 *
 * Moves jobs from a single admission queue of the executor. 
 * Jobs must acquire the required resource locks before being able to execute. 
 * A job will be assigned a processor id equal to its last required resource 
 * from the jobs file.
 *
 * Note: You do not need to spawn any new threads in here to simulate the processors.
 * When a job acquires all its required resources, it will execute do_stuff.
 * When do_stuff is finished, the job is considered to have completed.
 *
 * Once a job has completed, the admission thread must be notified since room
 * just became available in the queue. Be careful to record the job's completion
 * on its assigned processor and keep track of resources utilized. 
 *
 * Note: No printf statements are allowed in your final jobs.c code, 
 * other than the one from do_stuff!
 */
void *execute_jobs(void *arg) {
    struct admission_queue *q = arg;

    //Keep looping to execute jobs until there are jobs in admission queue or pending admission
    while(q != NULL){

	pthread_mutex_lock(&(q->lock));

	//Only return if there's no job pending to admit into this queue 
	//AND there are no jobs currently in queue
	if (q->num_admitted == 0 && q->pending_admission == 0){
    	    pthread_mutex_unlock(&(q->lock));
    	    return NULL;
	}

	//Wait if the admission queue is empty so it has currently has no jobs to execute
	while(q->num_admitted == 0){
	    pthread_cond_wait(&(q->admission_cv), &(q->lock));
	}
	
    	//Assign the current_job to first one from admission_queue
    	struct job *current_job = q->admitted_jobs[q->head];
	//Update values for the current admission_queue 
	q->head = (q->head + 1) % QUEUE_LENGTH;
	q->num_admitted -= 1;
        if(q->capacity < QUEUE_LENGTH){
		q->capacity += 1;
	}
	
	//Store the required resources into resource_list and sort it to get order for acquiring resources
	int *resource_list = current_job->resources;
    	qsort((resource_list), current_job->num_resources, sizeof(int), &compare);
    	
	//Acquire the required resource locks and update resource_utilization_check
	for(int j=0;j<current_job->num_resources;j++){
    		pthread_mutex_lock(&(tassadar.resource_locks[resource_list[j]]));
		tassadar.resource_utilization_check[resource_list[j]]--;
    	}

	//Set the oredered resource_list to current_job's resources so that processor id will be set to last 
	//required resource
	current_job->resources = resource_list;
	assign_processor(current_job);

    	//do stuff (execution)
    	do_stuff(current_job);

	//Update the processor records
	struct processor_record *complete_q = &(tassadar.processor_records[current_job->processor]);

    	pthread_mutex_lock(&(complete_q->lock));
	//Set the next value of the currently executed job to the list of previously completed jobs
	current_job->next = complete_q->completed_jobs;
	//Set the head of the completed_jobs list to the currently executed job and update num_completed
	complete_q->completed_jobs = current_job;
	complete_q->num_completed += 1;
    
    	pthread_mutex_unlock(&(complete_q->lock));

    	//Unlock resources
    	for(int j=0;j<current_job->num_resources;j++){
    		pthread_mutex_unlock(&(tassadar.resource_locks[resource_list[j]]));
    	}

	//Unlock and signal execution_cv that the admission_queue is not full
	pthread_cond_signal(&q->execution_cv);
	pthread_mutex_unlock(&q->lock);
    	
    }

    return NULL;
}
