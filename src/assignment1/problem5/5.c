#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main(int argc, char** argv)
{
    int pid;
    int pfd[2];
    pipe(pfd);
    pid=fork();
    

    //printf("%s %s\n",argv[1],argv[2] );
    if(pid==0)
    {
    	close(pfd[0]);
    	dup2(pfd[1],1);
    	printf("%s %s\n",argv[1],argv[2] );
    	execlp("/bin/grep","grep","-n",argv[1],argv[2],NULL);

    	exit(0);
    }
    else
    {
    	wait(NULL);
    	close(pfd[1]);
    	dup2(pfd[0],0);
    	execlp("cut","cut","-f1","-d:",NULL);
    	exit(0);
    }
} 
