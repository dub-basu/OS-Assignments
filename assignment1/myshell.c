#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

char USER[100];
char CWD[500];
int EXECUTING_SUBPROCESS = -1;

void setup(){
    getlogin_r(USER, sizeof(USER)/sizeof(USER[0]));
    getcwd(CWD, sizeof(CWD)/sizeof(CWD[0]));
}

void printPrompt(){
    setup();
    printf("%s%s:%s%s $ ","\x1B[32m",USER,"\x1B[37m",CWD);
    fflush(stdout);
}

void execCommand(){
    int controlW;
    char command[1000];
    command[0] = '\0';
    scanf("%[^\n]s", command);
    getchar(); // Hack to get the trailing newline char
    fflush(stdin);  
    char *split[1000];
    for (int i = 0; i < 1000; i++ ){ split[i] = NULL; }
    
    char* token = strtok(command," ");
    int i = 0;
    while(token != NULL){
        //printf("%s\n", token);
        split[i] = malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(split[i],token);
        token = strtok((char*)NULL," ");
        i++;
    }

    if(split[0] == NULL){
        return ;
    }
    
    if( strcmp(split[0],"exit") == 0){
        exit(0);
    }  
    else if (strcmp(split[0], "cd") == 0){
        if(split[1] == NULL){
            //printf("Please specify a directory\n");
            char homepath[100] = "/home/";
            strcat(homepath,USER);
            chdir(homepath);
        }
        else{
            int ret = chdir(split[1]);
            if(ret == -1){
                printf("Location doesn't exist\n");
            }
        }
    }
    else{
        int pid = fork();
        if(pid == 0){
            execvp(split[0], split);
            printf("-------Error executing command-------\n"); //execvp overwrites process image. 
                                                               //Shouldn't execute till execvp fails;
            exit(1);
        }
        else{
            EXECUTING_SUBPROCESS = pid;
            wait(&controlW);
            for(int i = 0; i < 1000 && split[i] != NULL; i++){
                free(split[i]);
            }
        }
    }
}

void SIGINT_HANDLER(int sig){
    printf("\n");
    if(EXECUTING_SUBPROCESS != -1){
        kill(EXECUTING_SUBPROCESS, SIGKILL);
        EXECUTING_SUBPROCESS = -1;
    }
    else{
        printPrompt();
    }
    fflush(stdout); 
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