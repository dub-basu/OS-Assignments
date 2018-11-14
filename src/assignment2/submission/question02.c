#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define N 8

// Possible taskStates
// ALLOW_RUN if all resources granted
#define ALLOW_RUN 0
// RUNNING if resources have been granted
#define RUNNING 1
// Set by resource allocator if it wants resources from a process
#define REQ_BACK_RESOURCE 2
// If insufficient resources present
#define RESOURCES_INSUFFICIENT 3
// If Process has run 1 time
#define FINISH_ITERATION 4
// If all iterations are done
#define FINISH_K_ITERATION 5

// Resource Vector Struct
struct resourceVector{
    int a, b, c, d;
};
typedef struct resourceVector resourceVector;

// Semaphores for all shared objects that need synchronisation
sem_t taskStatusSemaphore[N];
sem_t resourceMovement;
sem_t CPU;

// Global Vars for ease
int taskStatus[N];
resourceVector alloted[N];
resourceVector working;
resourceVector requirements[N];

int k;


resourceVector initVector(int a, int b, int c, int d){
    resourceVector ret;
    ret.a = a;
    ret.b = b;
    ret.c = c;
    ret.d = d;
    return ret;
}

void printResourceVector(resourceVector r){
    printf( "a : %d | b : %d | c : %d | d : %d \n", \
            r.a, r.b, r.c, r.d );
}

void* myProcess(void* id_void){
    int id  = *(int*)id_void;
    //printf("Process %d initialized\n", id);
    int numRun = 0;
    
    while(1){
        sem_wait(&taskStatusSemaphore[id]);
        //printf("HERE3 %d\n", id);
        if(taskStatus[id] == ALLOW_RUN){
            sem_wait(&CPU);
            taskStatus[id] = RUNNING;
            if(alloted[id].a < requirements[id].a || 
               alloted[id].b < requirements[id].b ||
               alloted[id].c < requirements[id].c ||
               alloted[id].d < requirements[id].d ){
                   printf("\n\n\nERROR\n\n\n");
            }
            if(working.a < 0 || working.b < 0 || working.c < 0 || working.d < 0){
                printf("\n\nERROR\n\n");
            }
            printf("Process %d is running. \n", id);
            sem_post(&taskStatusSemaphore[id]);
            

            // Sleep for random number of times
            //sleep(rand()%3);

            sem_wait(&taskStatusSemaphore[id]);
            sem_post(&CPU);
            taskStatus[id] = FINISH_ITERATION;
            printf( "Process %d completed the %d th iteration\n",\
                     id, numRun + 1);
            numRun++;
            if(numRun == k) { 
                taskStatus[id] = FINISH_K_ITERATION;
                sem_post(&taskStatusSemaphore[id]);
                break; 
            }
            sem_post(&taskStatusSemaphore[id]);
        }
        else if (taskStatus[id] == REQ_BACK_RESOURCE){
            printf("Resource requested back from Process %d\n", id);
            int a_ret = (rand()%3) < 2;
            int b_ret = (rand()%4) < 3;
            int c_ret = (rand()%5) < 3;
            int d_ret = (rand()%3) < 2;

            sem_wait(&resourceMovement);
            resourceVector returned = initVector(
                a_ret && alloted[id].a,
                b_ret && alloted[id].b,
                c_ret && alloted[id].c,
                d_ret && alloted[id].d
            );

            if(returned.a || returned.b || returned.c || returned.d){
                printf("Process %d returned ", id);
                printResourceVector(returned);
                working.a += returned.a;
                alloted[id].a -= returned.a;
                working.b += returned.b;
                alloted[id].b -= returned.b;
                working.c += returned.c;
                alloted[id].c -= returned.c;
                working.d += returned.d;
                alloted[id].d -= returned.d;
                taskStatus[id] = RESOURCES_INSUFFICIENT;
            }
            else{
                printf("Process %d returned no resources\n", id);
                taskStatus[id] = ALLOW_RUN;
            }
            sem_post(&resourceMovement);
            sem_post(&taskStatusSemaphore[id]);
        }
        else{
            sem_post(&taskStatusSemaphore[id]);
        }
    }
    
    sem_wait(&taskStatusSemaphore[id]);
    sem_wait(&resourceMovement);
    printf("Process %d is over. Has returned all resources.\n", id);
    //taskStatus[id] = FINISH_K_ITERATION;
    
    working.a += alloted[id].a;
    alloted[id].a = 0;
    working.b += alloted[id].b;
    alloted[id].b = 0;
    working.c += alloted[id].c;
    alloted[id].c = 0;
    working.d += alloted[id].d;
    alloted[id].d = 0;
    //printResourceVector(working);

    sem_post(&resourceMovement);
    sem_post(&taskStatusSemaphore[id]);

    pthread_exit(0);
}

int main(){
    srand(time(NULL));
    for(int i = 0; i < N; i++){
        sem_init(&taskStatusSemaphore[i],0,1);
    }
    sem_init(&resourceMovement,0,1);
    sem_init(&CPU, 0, 1);

    resourceVector total;
    scanf("%d", &k);
    scanf("%d %d %d %d", &total.a, &total.b, &total.c, &total.d);

    if(total.a < 1 || total.b < 1 || total.c < 1 || total.d < 1 ){
           printf("CANNOT RUN ALL PROCESSES DUE TO INSUFFICIENT RESOURCES\n");
           return 1;
    }
    printf("Each process will run %d times\nResources :: ", k);
    printResourceVector(total);

    working = total;

    // Initialize the values;
    pthread_t tasks[N];
    for(int i = 0; i < N; i++)  
        { taskStatus[i] = RESOURCES_INSUFFICIENT; }
    
    // Create the n threads to simulate processes.
    //resourceVector requirements[N];
    requirements[0] = initVector(1,1,1,0);
    requirements[1] = initVector(0,1,1,1);
    requirements[2] = initVector(1,0,1,1);
    requirements[3] = initVector(1,1,0,1);
    requirements[4] = initVector(1,0,0,0);
    requirements[5] = initVector(0,1,0,0);
    requirements[6] = initVector(0,0,1,0);
    requirements[7] = initVector(0,0,0,1);
    int arr_id[] = {0,1,2,3,4,5,6,7};

    for(int i = 0; i < N; i++){
        pthread_create(&tasks[i], NULL, &myProcess,(void*) &arr_id[i]);
    }

    // This part acts as the resource allocator
    while (1){
        int allDone = 1;
        for(int i = 0; i < N; i++){
            //printf("HERE1\n");
            sem_wait(&(taskStatusSemaphore[i]));
            if( taskStatus[i] != FINISH_K_ITERATION){
                allDone = 0;
            }
            if( taskStatus[i] == RESOURCES_INSUFFICIENT){
                // printf("HERE2 %d\n", i);
                sem_wait(&resourceMovement);
                // printf("HERE %d\n", i);
                resourceVector need = initVector(
                    requirements[i].a - alloted[i].a,
                    requirements[i].b - alloted[i].b,
                    requirements[i].c - alloted[i].c,
                    requirements[i].d - alloted[i].d
                );
                
                if( working.a >= need.a && working.b >= need.b &&
                    working.c >= need.c && working.d >= need.d){

                    working.a -= need.a;
                    alloted[i].a += need.a;
                    working.b -= need.b;
                    alloted[i].b += need.b;
                    working.c -= need.c;
                    alloted[i].c += need.c;
                    working.d -= need.d;
                    alloted[i].d += need.d;
                    taskStatus[i] = ALLOW_RUN;
                }
                
                sem_post(&resourceMovement);
            }
            else if( taskStatus[i] == FINISH_ITERATION){
                taskStatus[i] = REQ_BACK_RESOURCE;
            }
            sem_post(&taskStatusSemaphore[i]);
        }
        if(allDone == 1){
            printf("All Processes run k times\n");
            break;
        }
    }
}