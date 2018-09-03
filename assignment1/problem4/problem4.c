#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "utilities.h"

void kill_process(int pid){
    /* Kill process with pid = 'pid' */
    if(pid == -1){
        printf("No process killed\n");
        return;
    }

    pid_t pid_execution = fork();
    if(pid_execution < 0){
        perror("fork failed");
    } else if(pid_execution == 0){
        /* Child process to execute kill command */
        char proc[4];
        sprintf(proc, "%d", pid);

        if( execlp("kill", "kill", "-9", proc, NULL) == -1){
            perror("cannot kill\n");
        }
    } 
    int status;
    waitpid(pid, &status, 0);
    printf("\n");
}

void top_processes(int k){
    /* Display top k processes consuming CPU */
    
    int pfd[2];
    pipe(pfd);

    int pid1 = fork();

    if(pid1 == -1){
        perror("fork failed");
    } else if(pid1 == 0){
        /* child1: Execute ps command */
        close(pfd[0]);
        close(1);
        dup(pfd[1]);

        char* flag = "-e";
        char* fields_displayed = "pid ppid \%cpu \%mem comm etime";

        execlp("ps","ps",
            flag,
            "-o", fields_displayed,
            "--sort", "-\%cpu",
            NULL
        );
    }  

    int pid2 = fork();
    int status1;

    if(pid2 == -1){
        perror("fork failed");
    } else if(pid2 == 0){
        /* child2: Execute head command */
        
        waitpid(pid1, &status1, 0); 
        close(pfd[1]);
        close(0);
        dup(pfd[0]);

        char number_of_processes[4];
        sprintf(number_of_processes, "%d", k+1);
        
        execlp("head", "head","-n", number_of_processes, NULL);
    } 
    int status2;
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);
}

int main(){

    int n,k,r,qid,pid_to_kill=-1;
    pid_t pid;

    /* Take inputs: n, k, r */
    printf("Enter n (time gap in seconds):\t\t");
    scanf("%d",&n);
    printf("Enter k (number of processes):\t\t");
    scanf("%d",&k);
    printf("Enter r (number of repetetions):\t");
    scanf("%d",&r);

    /* Create message queue */
    qid = msgget(MSG_KEY, IPC_CREAT | 0666);

    pid = fork();
    if(pid == -1){
        perror("fork failed");
    } else if (pid == 0) {
        /* Child process.*/
        int p_status;
        int ppid = getppid();
        int count = 0;

        while(1){
            wait(&p_status);
            printf("\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
            while(count < r){
                top_processes(k);
                count++;
                if(count < r) sleep(n);
                printf("\n");
            }
            printf("Enter PID to kill:\n");
            pid_to_kill = get_pid_to_kill(qid);
            kill_process(pid_to_kill);
            count = 0;
        }
    }

    /* Parent process. Takes input and passes to message queue. */
    int status;
    signal(SIGINT, handler);
    while(1){
        scanf("%d",&pid_to_kill);
        push_pid_to_mqueue(qid, pid_to_kill);
    }
    wait(&status);
    exit(0);
    
}
