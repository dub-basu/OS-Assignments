#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "logger.h"
#include "get_process.h"
#include "events.h"

Process process_table[100];
int number_of_processes = 0;

EventQueue fcfs_queue = {0, FCFS, false, 0, 0};

int current_time = 0;
int CPU = -1;

void create_arrival_events(){    
    
    int i;
    for(i = 0;i < number_of_processes; i++){

        int n = fcfs_queue.number_of_events;

        fcfs_queue.events[n].pid = process_table[i].pid;
        fcfs_queue.events[n].event_type = Arrival;
        fcfs_queue.events[n].status = Awaited;
        fcfs_queue.events[n].time_of_occurence = process_table[i].arrival_time;

        fcfs_queue.number_of_events++;
    }
}

void run_fcfs_queue(int number_of_processes){

    int i;
    build_heap(&fcfs_queue, process_table);

    while(fcfs_queue.number_of_events >= 0){
        // loop till events are left
        Event retrieved = retrieve_event(&fcfs_queue, process_table);
        if(retrieved.pid == -1) break;

        current_time += process_table[retrieved.pid - 1].cpu_burst;
        
        Event new_event;
        new_event = retrieved;

        new_event.time_of_occurence = current_time;
        
        new_event.event_type = Allotted;
        fcfs_queue.completed_events[fcfs_queue.number_of_completed_events] = new_event;
        fcfs_queue.number_of_completed_events++;
        CPU = new_event.pid;

        new_event.event_type = CPUBurstComplete;
        fcfs_queue.completed_events[fcfs_queue.number_of_completed_events] = new_event;
        fcfs_queue.number_of_completed_events++;
        CPU = -1;

        process_table[new_event.pid - 1].termination_time = current_time;
    }


    for(i = 0;i < fcfs_queue.number_of_completed_events; i++){
        // Traverse through completed events
        Event eve;
        eve = fcfs_queue.completed_events[i];

        char type[40], log_msg[100];
        switch(eve.event_type){
            case 0: strcpy(type, "arrived at ready queue "); break;
            case 1: strcpy(type, "allotted CPU "); break;
            case 2: strcpy(type, "CPU Burst complete "); break;
            case 3: strcpy(type, "timer expired, back in ready queue "); break;
        }
        sprintf(log_msg, "%d\tProcess %d %s", eve.time_of_occurence, eve.pid, type);
        logger("FCFS", log_msg);
    }
}

int main(){

    int i = 0;
    char log_message[50];

    number_of_processes = create_pcb(process_table);
    
    create_arrival_events();

    run_fcfs_queue(number_of_processes);
    
    int total_wait_time = 0;
    for(i = 0; i < number_of_processes; i++){
        int start_time, end_time, cpu_time;
        start_time = process_table[i].arrival_time;
        end_time = process_table[i].termination_time;
        cpu_time = process_table[i].cpu_burst;
        process_table[i].wait_time = end_time - start_time - cpu_time;
        total_wait_time += process_table[i].wait_time;
    }
    int awt = total_wait_time / number_of_processes;
    printf("Average waiting time = %d\n", awt);

    return 0;
}
