#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

char USER[100];
char CWD[500];
// int EXECUTING_SUBPROCESS = -1;

void setup(){
    getlogin_r(USER, sizeof(USER)/sizeof(USER[0]));
    getcwd(CWD, sizeof(CWD)/sizeof(CWD[0]));
}

void printPrompt(){
    setup();
    //echo -e "text \033[1mbold\033[0m text"
    printf("%s%s:%s%s $ ","\x1B[32m",USER,"\x1B[37m",CWD);
    fflush(stdout);
}

void execCommand(){
    char command[1000] = "\0";
    scanf("%[^\n]s", command);
    getchar();
    fflush(stdin);
    //printf("command execed was %s\n", command);
    if( strcmp(command,"exit") == 0 )
        exit(0);
    else{
        
    }
    command[0] = '\0';
}

void SIGINT_HANDLER(int sig){
    //printf("LOL");
    fflush(stdout);
    printf("\n");
    printPrompt();
}

int main(){
    signal(SIGINT, SIGINT_HANDLER);
    while(1){
        //printf("\n");
        printPrompt();
        execCommand();
    
    }
    
    return 0;
}