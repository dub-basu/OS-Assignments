#include <stdio.h>
#include <unistd.h>

#define MY_FORK 363
void myFork(void){
    syscall(MY_FORK);
}

int main(){
    myFork();
    printf("This should be printed twice\n");
}
