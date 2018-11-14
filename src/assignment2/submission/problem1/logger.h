#ifndef LOGGER_H
#define LOGGER_H

#include<time.h>

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

    if(strcmp("FCFS", tag) != 0 && strcmp("RR", tag) != 0) return;
    printf(ANSI_COLOR_GREEN "%s\t" ANSI_COLOR_RESET,  tag);
    // printf(ANSI_COLOR_BLUE "%s\t" ANSI_COLOR_RESET, t);
    printf("%s\n",  message);

}

#endif /* LOG_H */