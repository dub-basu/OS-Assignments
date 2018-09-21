#define MSG_BUFF_SIZE 40
#define MSG_TYPE 1
#define MSG_KEY 1123

struct msgbuf {
    long mtype;
    char mtext[MSG_BUFF_SIZE];
};

void handler(int sig){
    printf("\n\nClean exit. Bye bye.\n");
    exit(0);
}

int to_int(char* word){
    char* ptr;
    int ret;
    ret = (int)strtol(word,&ptr,10);
    return(ret);
}

void push_pid_to_mqueue(int qid, int pid){
    /* Push pid to the message queue as a string */
    struct msgbuf msg;
    msg.mtype = MSG_TYPE;
    snprintf(msg.mtext, sizeof(msg.mtext), "%d", pid);
    if (msgsnd(qid,(void *) &msg, sizeof(msg.mtext),0) == -1){
        perror("msgsnd error");
        exit(EXIT_FAILURE);
    }
}

int get_pid_to_kill(int qid){
    /* Read from message queue and returns an integer */
    struct msgbuf msg;
    if (msgrcv(qid, (void *) &msg, sizeof(msg.mtext), MSG_TYPE,
                MSG_NOERROR ) == -1){
        if (errno != ENOMSG) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }
    } else {
        int p = to_int(msg.mtext);
        return(p);
    }
}

void remove_queue(int qid){
    int status;
    char queue_id[4];
    sprintf(queue_id, "%d", qid);
    pid_t pid = fork();

    if(pid == 0){
        close(2);
        if( execlp("ipcrm", "ipcrm","-Q", queue_id, NULL) == -1); 
        exit(0);  
    }
    wait(&status);
}