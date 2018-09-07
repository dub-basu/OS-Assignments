#include <stdio.h>
#include <unistd.h>

int main(){
    myFork();
    printf("This should be printed twice\n");
}
