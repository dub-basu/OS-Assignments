# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>

int main(){
    char* environment[] = {getenv("PATH"), NULL};
    char* args[] = {" -la", NULL};
    //execlp("ls","myshell", args);
    printf ("PATH : %s". environment[0]);
    execvpe("ls", args, environment);
    //execlp
    //printf("1000000000000\n");
    return 0;
}