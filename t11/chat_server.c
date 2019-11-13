#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include "socket.h"


#ifndef PORT
  #define PORT 50001
#endif
#define MAX_BACKLOG 5
#define MAX_CONNECTIONS 4
#define BUF_SIZE 128


struct sockname {
    int sock_fd;
    char *username ;
};


/* Accept a connection. Note that a new file descriptor is created for
 * communication with the client. The initial socket descriptor is used
 * to accept connections, but the new socket is used to communicate.
 * Return the new client's file descriptor or -1 on error.
 */
void remove_newline(char * str){
    for(int i =0;i < strlen(str);i++){
        if(str[i] == '\n'){
            str[i] = '\0';
        }
    }
}
void remove_networknewline(char * str){
    for(int i =0;i < strlen(str);i++){
        if(str[i] == '\r'){
            str[i] = ' ';
        }
    }
}
int setup_new_client(int fd, struct sockname *usernames) {
    int user_index = 0;
    while (user_index < MAX_CONNECTIONS && usernames[user_index].sock_fd != -1) {
        user_index++;
    }

    int client_fd = accept_connection(fd);
    if (client_fd < 0) {
        return -1;
    }

    if (user_index >= MAX_CONNECTIONS) {
        fprintf(stderr, "server: max concurrent connections\n");
        close(client_fd);
        return -1;
    }

    usernames[user_index].sock_fd = client_fd;
    usernames[user_index].username = NULL;
    return client_fd;
}


/* Read a message from client_index and echo it back to them.
 * Return the fd if it has been closed or 0 otherwise.
 */
int read_user(int client_index, struct sockname *usernames) {
    int fd = usernames[client_index].sock_fd;
    char * buf = malloc(sizeof(char) * (BUF_SIZE + 1));

    int num_read = read(fd, buf, BUF_SIZE);
    buf[num_read] = '\0';
    
    if (num_read == 0 || write(STDOUT_FILENO, buf, strlen(buf)) != strlen(buf)) {
        return fd;
    }
    
    
        usernames[client_index].username = buf;
    
    return 0;
}

int read_from(int client_index, struct sockname *usernames) {
    int fd = usernames[client_index].sock_fd;
    char buf[BUF_SIZE + 1];
    char msg[(BUF_SIZE + 1) * 2];
    memset(buf,0,BUF_SIZE);
   memset(msg,0,(BUF_SIZE + 1) * 2);
    int num_read = read(fd, &buf, BUF_SIZE);
    remove_networknewline(&(buf[0]));
    buf[BUF_SIZE] = '\0';
    snprintf(msg,(BUF_SIZE + 1) * 2,"%s: %s",usernames[client_index].username, buf);
    msg[(BUF_SIZE + 1) * 2] = '\0';
    remove_networknewline(&(msg[0]));
    if (num_read == 0) {
        usernames[client_index].sock_fd = -1;
        return fd;
    }
    
    for(int i = 0; i < MAX_CONNECTIONS; i++)
    {
        if (client_index == i)
        {
           printf("%s", msg);
        }
        else 
        {
            
            write(usernames[i].sock_fd,msg,(BUF_SIZE + 1) * 2);
        }

    }
   memset(buf,0,BUF_SIZE);
   memset(msg,0,(BUF_SIZE + 1) * 2);
    return 0;
}



int main(void) {
    // This line causes stdout not to be buffered.
    // Don't change this! Necessary for autotesting.
    setbuf(stdout, NULL);
    struct sockaddr_in *self = init_server_addr(PORT);
    int sock_fd = setup_server_socket(self, MAX_BACKLOG);

    // Create a list of chat client users.
    struct sockname usernames[MAX_CONNECTIONS];
    for (int index = 0; index < MAX_CONNECTIONS; index++) {
        usernames[index].sock_fd = -1;
        usernames[index].username = NULL;
    }

    // The client accept - message accept loop. First, we prepare to listen to multiple
    // file descriptors by initializing a set of file descriptors.
    int max_fd = sock_fd;
    fd_set all_fds, listen_fds, write_fds;
    FD_ZERO(&all_fds);
    FD_SET(sock_fd, &all_fds);
    printf("[ %d ]\n",getpid());//remove this!!!!!!!!!!!!!
    while (1) {
        // select updates the fd_set it receives, so we always use a copy and retain the original.
        listen_fds = all_fds;
        write_fds = all_fds;
        int nready = select(max_fd + 1, &listen_fds, &write_fds, NULL, NULL);
        if (nready == -1) {
            perror("server: select");
            exit(1);
        }

        // Is it the original socket? Create a new connection ...
        if (FD_ISSET(sock_fd, &listen_fds)) {
            int client_fd = setup_new_client(sock_fd, usernames);
            if (client_fd < 0) {
                continue;
            }
            if (client_fd > max_fd) {
                max_fd = client_fd;
            }
            FD_SET(client_fd, &all_fds);
            printf("Accepted connection\n");
        }
      
        //char buf[BUF_SIZE];
        // Next, check the clients.
        // NOTE: We could do some tricks with nready to terminate this loop early.
        for (int index = 0; index < MAX_CONNECTIONS; index++) {
            if (usernames[index].sock_fd > -1 && FD_ISSET(usernames[index].sock_fd, &listen_fds)) {
                // Note: never reduces max_fd
                if (usernames[index].username == NULL) {
                   read_user(index, usernames);
                   remove_newline(usernames[index].username);
                }
                else{
                    int client_closed = read_from(index, usernames);
                    if (client_closed > 0) {
                        FD_CLR(client_closed, &all_fds);
                        usernames[index].username = NULL;
                        free(usernames[index].username);
                        close(client_closed);
                        printf("Client %d disconnected\n", client_closed);
                    
                    }
                    
            }
        }
    }
    }
    // Should never get here.
    return 1;
}
