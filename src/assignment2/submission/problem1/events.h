#ifndef EVENTS_H
#define EVENTS_H
#define left(x) (2*x + 1)
#define right(x) (2*x + 2)
#define parent(x) (x-1)/2

typedef struct Event {
    int pid;
    enum { Arrival, Allotted, CPUBurstComplete, TimerExpired } event_type;
    enum { Awaited, Occured } status;
    int time_of_occurence;
    int priority;
} Event;

typedef struct EventQueue {
    int number_of_events;
    enum { FCFS, RoundRobin } sched_policy;
    enum { false, true } preemption;
    int time_quantum;
    int number_of_completed_events;
    int rr_prio;
    int number_of_arrival_events;
    Event events[100];
    Event rr_arrival_events[100];
    Event completed_events[100];
} EventQueue;

int compare(Event event1, Event event2, int policy, Process process_table[]){

    Process process1 = process_table[event1.pid - 1];
    Process process2 = process_table[event2.pid - 1];

    if(policy == FCFS){
        if(process1.arrival_time < process2.arrival_time) return 1;
        else if(process1.arrival_time > process2.arrival_time) return 0;
        else{
            if(process1.pid < process2.pid) return 1;
            else return 0;
        }
    } 
    
    else if (policy == RoundRobin) {
        if(event1.priority < event2.priority) return 1;
        else if(event1.priority > event2.priority) return 0;
        else {
            if(process1.arrival_time < process2.arrival_time) return 1;
            else if(process1.arrival_time > process2.arrival_time) return 0;
            else{
                if(process1.pid < process2.pid) return 1;
                else return 0;
            }
        }
    }
    return 0;
}

void heapify(EventQueue* event_queue, int i, Process process_table[]){
    Event   left_child = event_queue -> events[left(i)],
            right_child = event_queue -> events[right(i)],
            current = event_queue -> events[i];
    
    int best;
    if(left(i) < event_queue -> number_of_events && compare(left_child, current, event_queue -> sched_policy, process_table))
        best = left(i);
    else
        best = i;
    
    Event best_child = event_queue -> events[best];
    if(right(i) < event_queue -> number_of_events && compare(right_child, best_child, event_queue -> sched_policy, process_table))
        best = right(i);
    
    if(best != i){
        Event temp = event_queue -> events[best];
        event_queue -> events[best] = event_queue -> events[i];
        event_queue -> events[i] = temp;

        char log_msg[30];
        sprintf(log_msg, "Swap %d and %d", i, best);
        logger("HEAP", log_msg);
        
        heapify(event_queue, best, process_table);
    }
}

void build_heap(EventQueue* event_queue, Process process_table[]){
    int i;
    for(i = (event_queue -> number_of_events) / 2 - 1; i>=0; i--){
        heapify(event_queue, i, process_table);
    }
}

Event retrieve_event(EventQueue* event_queue, Process process_table[]){
    
    if(event_queue -> number_of_events <= 0){
        logger("ERROR", "No event available to retrieve. Returning NULL event");
        Event null_event = {-1};
        return(null_event);
    }

    Event first_event = event_queue -> events[0];
    Event last_event = event_queue -> events[event_queue -> number_of_events - 1];
    event_queue -> events[0] = last_event;

    char log_msg[30];
    sprintf(log_msg, "Popped process %d from queue", first_event.pid);
    logger("HEAP", log_msg);

    event_queue -> number_of_events --;
    
    
    if(event_queue -> sched_policy == FCFS){
        event_queue -> completed_events[event_queue -> number_of_completed_events] = first_event;
        event_queue -> number_of_completed_events ++;
    }

    heapify(event_queue, 0, process_table);
    return(first_event);
}

#endif /* EVENTS_H */