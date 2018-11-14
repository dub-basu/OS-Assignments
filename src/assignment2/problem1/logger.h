#ifndef LOGGER_H
#define LOGGER_H

#include<time.h>
// #include "events.h"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void logger(const char* tag, const char* message) {

    time_t now;
    time(&now);
    char* t = ctime(&now);
    t[strlen(t)-1] = '\0';

    if( strcmp("FCFS", tag) != 0 && 
        strcmp("RR", tag) != 0 &&
        strcmp("MISC", tag) != 0 &&
        strcmp("ERROR", tag) != 0
    ) return;
    printf(ANSI_COLOR_GREEN "%s\t" ANSI_COLOR_RESET,  tag);
    // printf(ANSI_COLOR_BLUE "%s\t" ANSI_COLOR_RESET, t);
    printf("%s\n",  message);
}

// void log_event(const char* policy, Event eve){
    // printf(ANSI_COLOR_GREEN "%s\t" ANSI_COLOR_RESET, policy);

    // char type[40], log_msg[100];
    // switch(eve.event_type){
    //     case 0: strcpy(type, "arrived at ready queue "); break;
    //     case 1: strcpy(type, "allotted CPU "); break;
    //     case 2: strcpy(type, "CPU Burst complete "); break;
    //     case 3: strcpy(type, "timer expired, back in ready queue "); break;
    //     case 4: strcpy(type, "preempted by another process "); break;
    // }
    // sprintf(log_msg, "%d\tProcess %d %s", eve.time_of_occurence, eve.pid, type);
    // // printf(ANSI_COLOR_BLUE "%s\t" ANSI_COLOR_RESET, eve.time_of_occurence);
    // // printf("%s\n", log_msg)
    // printf("%s\n", log_msg);
// }

#endif /* LOG_H */