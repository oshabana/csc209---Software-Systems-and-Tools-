#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "eratosthenes.h"
#define MAXSIZE 15000

pid_t make_stage(int n, int read_fd, int **fds) {
    int *actual_fds = *fds;
    char list[MAXSIZE];
    
    pipe(actual_fds);
    int pid = fork();

    if(pid > 0){
        read(read_fd,list,sizeof(char) * MAXSIZE);
        write(actual_fds[1],list,sizeof(char) * MAXSIZE);
        filter(n,actual_fds[0],actual_fds[1]);
        close(actual_fds[0]);
        close(actual_fds[1]);
        close(read_fd);

    }
    else
    {
        
        close(read_fd);
    }
    
    
    
    //close(actual_fds[0]);
    
    return pid;
}
