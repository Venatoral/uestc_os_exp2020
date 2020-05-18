/*---------------------------------------------------------------- 
// 版权所有。  
// 文件名： pipe.c
// 文件功能描述： C语言pipe初探
// author：马玏
// 时间：2020-5-18
//----------------------------------------------------------------*/
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
// true on DEBUG mod to see the run track
// #define DEBUG
int main() {
    int fd[2];
    // Create pipe
    if ( pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    // child  1 write
    int pid = fork();
    if(pid == 0) {
        char buf[128] = "Child 1 is sending a message\0";
        close(fd[0]);
        if( write(fd[1], buf, strlen(buf)) == -1 ) {
            perror("write");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
#ifdef DEBUG
    printf("before wait child2\n");
#endif
    // wait child 1 to write
    waitpid(pid, NULL, 0);
#ifdef DEBUG
    printf("after wait child2\n");
#endif
    int newFd[2];
    // Create pipe
    if (pipe(newFd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    // child  2 write
    pid = fork();
    if (pid == 0) {
        char buf[128] = "Child 2 is sending a message\0";
        close(fd[0]);
        if (write(newFd[1], buf, strlen(buf)) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
    // wait child 2 to finish write
#ifdef  DEBUG
    printf("before wait child2...\n");
#endif
    waitpid(pid, NULL, 0);
#ifdef DEBUG
    printf("after wait child2...\n");
#endif
    // Read message from child1 and child2
    char msg1[128], msg2[128];
    // read child 1
    close(fd[1]);
#ifdef DEBUG
    printf("Before read from child1\n");
#endif
    if (read(fd[0], msg1, 128) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }
#ifdef DEBUG
    printf("After read from child1\n");
#endif
    // read child2
#ifdef DEBUG
    printf("Before read from child2\n");
#endif
    if (read(newFd[0], msg2, 128) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }
#ifdef DEBUG
    printf("After read from child2\n");
#endif

// show msg1 and msg2;
printf("\033[1;31mReceive msg:\nmsg1:%s\nmsg2%s\n", msg1, msg2);
return 0;
}