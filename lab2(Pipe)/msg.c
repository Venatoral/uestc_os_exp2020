/*---------------------------------------------------------------- 
// 版权所有。  
// 文件名： msg.c
// 文件功能描述： C语言消息队列初探
// author：马玏
// 时间：2020-5-11
//----------------------------------------------------------------*/
#include <stdio.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

#define DEBUG
#undef DEBUG

typedef struct _msg {
    long type;
    char body[256];
}msg;
int main() {
    // get key_t for msgget
    int fid = ftok("./", 0);
    // create ipc
    int mid;
    if ( (mid = msgget(fid, 0666|IPC_CREAT) ) == -1 ) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    
    // fork a thread to write msg into ipc
    int pid = fork();
    if(pid == 0) {
        msg* m = calloc(1, sizeof(msg));
        strcpy(m->body, "Hello, I have written this msg into msgq,can you see it?\0");
        m->type = 1;
#ifdef DEBUG
    printf("before send msg\n");
#endif
        if ( msgsnd(mid, m, sizeof(msg), 0) == -1 ) {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }

#ifdef DEBUG
    printf("after send msg\n");
#endif
        free(m);
        exit(EXIT_SUCCESS);
    }
    // wait child to finish write
    waitpid(pid, NULL, 0);
    msg* recv = calloc(1, sizeof(msg));
#ifdef DEBUG
    printf("before receive from msgq\n");
#endif
    if ( msgrcv(mid, recv, sizeof(msg), 1, 0) == -1 ) {
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }
#ifdef DEBUG
    printf("after receive from msgq\n");
#endif
    printf("\033[1;31mRecv: %s\n", recv->body);
    // remove ipc
    if (msgctl(mid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }
    return 0;
}