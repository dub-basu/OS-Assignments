#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "logger.h"
#include "get_process.h"
#include "events.h"

#define TIMEQUANTUM 4
#define TOPQUEUETHRESHOLD 8

Process process_table[100];
int number_of_processes = 0;
int CPU = -1;

int switch_control_for = 0;
int rr_thread_id = 0;
int fcfs_thread_id = 0;

EventQueue fcfs_queue = {0, FCFS, false, 0, 0};
EventQueue rr_queue = {0, RoundRobin, true, TIMEQUANTUM, 0, 0};

int current_time = 0;

void create_arrival_events(){    

    int i;
    for(i = 0;i < number_of_processes; i++){

        if(process_table[i].cpu_burst > TOPQUEUETHRESHOLD){
            int n = fcfs_queue.number_of_events;
            fcfs_queue.events[n].pid = process_table[i].pid;
            fcfs_queue.events[n].event_type = Arrival;
            fcfs_queue.events[n].status = Awaited;
            fcfs_queue.events[n].time_of_occurence = process_table[i].arrival_time;

            fcfs_queue.number_of_events++;
        } else {
            int n = rr_queue.number_of_arrival_events;

            rr_queue.rr_arrival_events[n].pid = process_table[i].pid;
            rr_queue.rr_arrival_events[n].event_type = Arrival;
            rr_queue.rr_arrival_events[n].status = Awaited;
            rr_queue.rr_arrival_events[n].time_of_occurence = process_table[i].arrival_time;

            rr_queue.number_of_arrival_events++;
        }
    }
}

int compar (const void* p1, const void* p2){
    Event event1 = *(Event*)p1;
    Event event2 = *(Event*)p2;

    if(process_table[event1.pid - 1].arrival_time < process_table[event2.pid - 1].arrival_time){
        logger("SORT", "left");
        return -1;
    } else if (process_table[event1.pid - 1].arrival_time > process_table[event2.pid - 1].arrival_time){
        logger("SORT", "right");
        return 1;
    } else {
        if(event1.pid < event2.pid) return -1;
        else if(event1.pid > event2.pid) return 1;
        else return 0;
    }    
}

void log_rr_events(){
    int i;
    for(i=0;i<rr_queue.number_of_completed_events;i++){
        // traverse thru completed events
        Event eve;
        eve = rr_queue.completed_events[i];

        char type[40], log_msg[100];
        switch(eve.event_type){
            case 0: strcpy(type, "arrived at ready queue "); break;
            case 1: strcpy(type, "allotted CPU "); break;
            case 2: strcpy(type, "CPU Burst complete "); 
                    process_table[eve.pid - 1].termination_time = eve.time_of_occurence;
                    break;
            case 3: strcpy(type, "timer expired, back in ready queue "); break;
        }
        sprintf(log_msg, "%d\tProcess %d %s", eve.time_of_occurence, eve.pid, type);
        logger("RR", log_msg);
    }
}

void log_fcfs_events(){
    int i;
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
            case 4: strcpy(type, "preempted by another process "); break;
        }
        sprintf(log_msg, "%d\tProcess %d %s", eve.time_of_occurence, eve.pid, type);
        logger("FCFS", log_msg);
    }
}

void run_queues(int number_of_processes){

    build_heap(&rr_queue, process_table);
    build_heap(&fcfs_queue, process_table);

    Event* arrival_event_ptr;
    arrival_event_ptr = rr_queue.rr_arrival_events;

    while(rr_queue.number_of_events > 0 || rr_queue.number_of_arrival_events > 0 || fcfs_queue.number_of_events > 0){
        build_heap(&rr_queue, process_table);
        if(rr_queue.number_of_events == 0){
            // printf("0 events in rr at time %d\n", current_time);
        }
        if(rr_queue.number_of_events > 0){
            // printf("first condition at time %d\n", current_time);
            // collect all events arrived till current time    
            while(1){
                
                if(rr_queue.number_of_arrival_events > 0)
                    if(arrival_event_ptr -> time_of_occurence <= current_time){
                        Event new_event;
                        new_event = *arrival_event_ptr;
                        new_event.priority = ++rr_queue.rr_prio;
                        
                        arrival_event_ptr++;
                        rr_queue.number_of_arrival_events--;
                        new_event.status = Occured;
                        rr_queue.completed_events[rr_queue.number_of_completed_events] = new_event;
                        rr_queue.number_of_completed_events++;
                        // log_event("RoundRobin", new_event);

                        new_event.event_type = Allotted;
                        new_event.status = Awaited;
                        rr_queue.events[rr_queue.number_of_events] = new_event;
                        rr_queue.number_of_events++;

                    } else break;
                else break;
            }

            // retrieve top event
            Event retrieved = retrieve_event(&rr_queue, process_table);
            if(retrieved.pid == -1) break;
            retrieved.time_of_occurence = current_time;
            retrieved.status = Occured;

            rr_queue.completed_events[rr_queue.number_of_completed_events] = retrieved;
            rr_queue.number_of_completed_events++;
            // log_event("RoundRobin", retrieved);

            Event new_event;
            new_event.pid = retrieved.pid;
            new_event.status = Awaited;
            int n = rr_queue.number_of_events;
            Process current_process = process_table[retrieved.pid - 1];

            if(retrieved.event_type == Allotted){
            
                CPU = retrieved.pid;
                if(current_process.time_left <= rr_queue.time_quantum){
                    current_time += current_process.time_left;
                    new_event.event_type = CPUBurstComplete;
                    new_event.priority = retrieved.priority;
                    new_event.time_of_occurence = current_time;

                    process_table[retrieved.pid - 1].time_left = 0;
                    process_table[retrieved.pid - 1].state = Running;
                } else { // process time_left > time quantum: timer expired 
                    current_time += rr_queue.time_quantum;
                    new_event.event_type = TimerExpired;
                    new_event.priority = retrieved.priority;
                    new_event.time_of_occurence = current_time;

                    process_table[retrieved.pid - 1].time_left -= rr_queue.time_quantum;
                    process_table[retrieved.pid - 1].state = Running;
                }
            } else if(retrieved.event_type == CPUBurstComplete) {
                process_table[retrieved.pid - 1].state = Terminated; 
                CPU = -1;
                continue;

            } else if(retrieved.event_type == TimerExpired) {
                new_event.event_type = Allotted;
                new_event.priority = ++rr_queue.rr_prio;
                process_table[retrieved.pid - 1].state = Ready;
                CPU = -1;
            }

            rr_queue.events[rr_queue.number_of_events] = new_event;
            rr_queue.number_of_events++;

        } else if(rr_queue.number_of_arrival_events > 0) {
            // printf("no event in rr queue but there in rr arrival queue at time %d\n", current_time);

            // rr event queue is empty
        
            int next_rr_arrival_time = arrival_event_ptr -> time_of_occurence;
            int next_fcfs_event_time;

            if(peek_event(&fcfs_queue, process_table).pid == -1) // null event
                next_fcfs_event_time = 5000;
            else
                next_fcfs_event_time = peek_event(&fcfs_queue, process_table).time_of_occurence;
            // printf("next fcfs event time = %d\n", next_fcfs_event_time);

            if(next_rr_arrival_time <= next_fcfs_event_time){

                Event new_event;
                new_event = *arrival_event_ptr;
                // logger("MISC", "IDLE");
                // printf("Current time: %d\n", current_time);
                // printf("Next arrival time: %d\n", new_event.time_of_occurence);
                // printf("IDLE for: %d\n\n", new_event.time_of_occurence - current_time);

                arrival_event_ptr++;
                rr_queue.number_of_arrival_events--;
                new_event.status = Occured;
                rr_queue.completed_events[rr_queue.number_of_completed_events] = new_event;
                rr_queue.number_of_completed_events++;
                // log_event("RoundRobin", new_event);

                new_event.event_type = Allotted;
                new_event.priority = ++rr_queue.rr_prio;
                new_event.status = Awaited;
                rr_queue.events[rr_queue.number_of_events] = new_event;
                rr_queue.number_of_events++;

                process_table[new_event.pid - 1].state = Ready;
                current_time = new_event.time_of_occurence;

            } else if(fcfs_queue.number_of_events > 0) {

                Event retrieved = retrieve_event(&fcfs_queue, process_table);
                if(retrieved.pid == -1) break;

                if(retrieved.event_type != Allotted){
                    fcfs_queue.completed_events[fcfs_queue.number_of_completed_events] = retrieved;
                    fcfs_queue.number_of_completed_events++;
                    // log_event("RoundRobin", retrieved);
                }

                int burst_time;
                burst_time = process_table[retrieved.pid - 1].time_left;

                if(burst_time + current_time < next_rr_arrival_time){
                    // no preemption
                    current_time += burst_time;
                
                    Event new_event;
                    new_event = retrieved;

                    new_event.time_of_occurence = current_time;
                    
                    new_event.event_type = Allotted;
                    fcfs_queue.completed_events[fcfs_queue.number_of_completed_events] = new_event;
                    fcfs_queue.number_of_completed_events++;
                    // log_event("RoundRobin", new_event);

                    new_event.event_type = CPUBurstComplete;
                    fcfs_queue.completed_events[fcfs_queue.number_of_completed_events] = new_event;
                    fcfs_queue.number_of_completed_events++;
                    // log_event("RoundRobin", new_event);
                    process_table[retrieved.pid - 1].time_left -= burst_time;

                    process_table[new_event.pid - 1].termination_time = current_time;

                } else {
                    // preempt fcfs with rr event
                    Event new_event;
                    new_event = retrieved;

                    if(current_time < retrieved.time_of_occurence)
                    current_time = retrieved.time_of_occurence;

                    burst_time = next_rr_arrival_time - current_time;

                    new_event.time_of_occurence = current_time;
                    new_event.status = Occured;
                    new_event.event_type = Allotted;
                    fcfs_queue.completed_events[fcfs_queue.number_of_completed_events] = new_event;
                    fcfs_queue.number_of_completed_events++;
                    // log_event("RoundRobin", new_event);
                    CPU = new_event.pid;

                    current_time += burst_time;

                    new_event.event_type = Preempted;
                    new_event.status = Occured;
                    new_event.time_of_occurence = current_time;
                    fcfs_queue.completed_events[fcfs_queue.number_of_completed_events] = new_event;
                    fcfs_queue.number_of_completed_events++;
                    // log_event("RoundRobin", new_event);
                    CPU = -1;

                    new_event.event_type = Allotted;
                    new_event.status = Awaited;
                    new_event.time_of_occurence = current_time;
                    fcfs_queue.events[fcfs_queue.number_of_events] = new_event;
                    fcfs_queue.number_of_events++;

                    process_table[new_event.pid - 1].time_left -= burst_time;
                    // printf("burst time left = %d\n",process_table[new_event.pid - 1].time_left);
                    build_heap(&fcfs_queue, process_table);

                }
            }

        } else if (fcfs_queue.number_of_events > 0){
            // rr completed, only fcfs left
            Event retrieved = retrieve_event(&fcfs_queue, process_table);
            if(retrieved.pid == -1) break;

            if(retrieved.event_type != Allotted){
                fcfs_queue.completed_events[fcfs_queue.number_of_completed_events] = retrieved;
                fcfs_queue.number_of_completed_events++;
                // log_event("RoundRobin", retrieved);
            }

            if(current_time < retrieved.time_of_occurence)
                current_time = retrieved.time_of_occurence;

            int burst_time;
            burst_time = process_table[retrieved.pid - 1].time_left;

            Event new_event;
            new_event = retrieved;

            // new_event.time_of_occurence = current_time;
            new_event.time_of_occurence = current_time;
            new_event.event_type = Allotted;
            fcfs_queue.completed_events[fcfs_queue.number_of_completed_events] = new_event;
            fcfs_queue.number_of_completed_events++;
            // log_event("RoundRobin", new_event);
            CPU = new_event.pid;

            current_time += burst_time;

            new_event.time_of_occurence = current_time;
            new_event.event_type = CPUBurstComplete;
            fcfs_queue.completed_events[fcfs_queue.number_of_completed_events] = new_event;
            fcfs_queue.number_of_completed_events++;
            // log_event("RoundRobin", new_event);
            CPU = -1;

            process_table[new_event.pid - 1].termination_time = current_time;
        }
    }
}

int main(){

    int i = 0;
    char log_message[50];

    number_of_processes = create_pcb(process_table);
    
    create_arrival_events();

    // sort arrival events in rr_queue
    qsort((void*)rr_queue.rr_arrival_events, rr_queue.number_of_arrival_events, sizeof(rr_queue.rr_arrival_events[0]), compar);

    // printf("%d\n", rr_queue.number_of_arrival_events);
    run_queues(number_of_processes);


    Event final_events[500];
    int number_of_final_events;

    for(i=0;i<rr_queue.number_of_completed_events;i++)
        final_events[number_of_final_events++] = rr_queue.completed_events[i];
    for(i=0;i<fcfs_queue.number_of_completed_events;i++)
        final_events[number_of_final_events++] = fcfs_queue.completed_events[i];

    log_rr_events();
    log_fcfs_events();

    int total_wait_time = 0;
    for(i = 0; i < number_of_processes; i++){
        int start_time, end_time, cpu_time;
        start_time = process_table[i].arrival_time;
        end_time = process_table[i].termination_time;
        cpu_time = process_table[i].cpu_burst;
        process_table[i].wait_time = end_time - start_time - cpu_time;
        total_wait_time += process_table[i].wait_time;
    }
    float awt = (float)total_wait_time / (float)number_of_processes;
    printf("\nAverage waiting time = %f\n", awt);

    

    print_waiting_times(process_table, number_of_processes);

    return 0;
}
