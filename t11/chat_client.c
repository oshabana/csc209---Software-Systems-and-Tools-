#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include "socket.h"

#ifndef PORT
  #define PORT 50006
#endif
#define BUF_SIZE 128
void remove_networknewline(char * str){
    for(int i =0;i < strlen(str);i++){
        if(str[i] == '\r'){
            str[i] = '\0';
        }
    }
}
int main(void) {
    char username[BUF_SIZE] = "\0";
    int sock_fd = connect_to_server(PORT, "127.0.0.1");
   // struct timeval *timeout;
   // timeout->tv_sec = 1;
    fd_set listenfds,allfds;
//fd_set writefds;
    //FD_ZERO(&allfds);
    
    // Read input from the user, send it to the server, and then accept the
    // echo that returns. Exit when stdin is closed.
    char buf[BUF_SIZE + 1];
   int num_read;
    int num_written;
    FD_ZERO(&allfds);
    //FD_SET(sock_fd, &writefds);
    FD_SET(sock_fd, &allfds);
    FD_SET(STDIN_FILENO, &allfds);
    
    while (1) {
        int num_fd;
        if(strcmp(username,"\0") == 0){
            printf("Username: \n");
            num_read = read(STDIN_FILENO, username, BUF_SIZE);
            if (num_read == 0) {
                break;
            }
            username[num_read] = '\0';         

            num_written = write(sock_fd, username, num_read);
            if (num_written != num_read) {
                perror("client: write");
                close(sock_fd);
                exit(1);
            }

            
        }
        if(STDIN_FILENO > sock_fd){
            num_fd = STDIN_FILENO +1 ;
        }
        else
        {
            num_fd = sock_fd + 1;
        }

        listenfds = allfds;

        int nready = select(num_fd, &listenfds, NULL, NULL, NULL);
        if(nready == -1){
            perror("select");
            exit(1);
        }
        
        if (FD_ISSET(sock_fd, &listenfds) != 0){
            num_read = read(sock_fd, buf, BUF_SIZE);
            if (num_read == 0) {
                break;
            }
            buf[num_read] = '\0'; 
            remove_networknewline(buf);
            printf("%s", buf);
            //FD_CLR(sock_fd, &allfds);
        }
        if (FD_ISSET(STDIN_FILENO, &listenfds) != 0){
            num_read = read(STDIN_FILENO, &buf, BUF_SIZE);
            if (num_read == 0) {
                break;
            }
            buf[num_read] = '\0';       

            num_written = write(sock_fd, &buf, num_read);
            if (num_written != num_read) {
                perror("client: write");
                close(sock_fd);
                exit(1);
            }

            //num_read = read(sock_fd, buf, BUF_SIZE);
            
            //FD_CLR(STDIN_FILENO, &allfds);
            }
           
    }

    close(sock_fd);
    return 0;
}
