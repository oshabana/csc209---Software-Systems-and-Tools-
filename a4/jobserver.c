#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include "socket.h"
#include "jobprotocol.h"
#define QUEUE_LENGTH 5
#define MAX_CLIENTS 20
#ifndef MAX_JOBS
    #define MAX_JOBS 32
#endif
#ifndef JOBS_DIR
    #define JOBS_DIR "jobs/"
#endif
int dead = 0;
int shutdown_sig = 0;

void child_handler(){
    dead =1;
    
}
void shutdown_server(){
    shutdown_sig = 1;
}
int main(void){
    // This line causes stdout and stderr not to be buffered.
    // Don't change this! Necessary for autotesting.
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    struct sockaddr_in *self = init_server_addr(PORT);
    int sock_fd = setup_server_socket(self, QUEUE_LENGTH);
    Client ** client_list = make_client_list(MAX_CLIENTS);
    JobNode ** job_list = create_joblist(MAX_JOBS);

    struct sigaction killact;
    killact.sa_handler = child_handler;
    killact.sa_flags = SA_RESTART;
    sigemptyset(&killact.sa_mask);
    sigaction(SIGCHLD,&killact,NULL);
    memset (&killact, 0, sizeof (killact));

    struct sigaction stopact;
    killact.sa_handler = shutdown_server;
    killact.sa_flags = 0;
    sigemptyset(&stopact.sa_mask);
    sigaction(SIGINT,&stopact,NULL);
    memset (&stopact, 0, sizeof (stopact));

    //int fd_list[MAX_CLIENTS];
    //JobNode * job_list[MAX_JOBS];

    /* TODO: Initialize job and client tracking structures, start
     * accepting connections. Listen for messages from both clients
     * and jobs. Execute client commands if properly formatted. 
     * Forward messages from jobs to appropriate clients. 
     * Tear down cleanly.
     * Here is a snippet of code to create the command of an executable
     * to execute:
     * char exe_file[BUFSIZE];
     * snprintf(exe_file, BUFSIZE, "%s/%s", JOBS_DIR, <job_command>);
     */
    //printf("[ %d ]\n",getpid()); // GET RID OF THIS LINE!!
    fd_set allfds, listenfds, writefds, errorfds;
    int maxfd = sock_fd;
    int fd[2];
    if(pipe(fd) < 0)
    {
        perror("pipe");
    }
    FD_ZERO(&allfds);
    FD_ZERO(&writefds);
    FD_ZERO(&errorfds);
    FD_SET(sock_fd, &allfds);
    FD_SET(fd[0],&allfds);
    FD_SET(fd[1],&allfds);
    while(shutdown_sig != 1){
        int client_fd = -1;
        listenfds = allfds;
        writefds = allfds;
        errorfds = allfds;
        int nready = select(maxfd + 1,&listenfds,&writefds,&errorfds,NULL);
        if (nready == -1) {
            perror("server: select");
        
        }
        //====================================================================
        if(FD_ISSET(sock_fd, &listenfds)){
            Client * this_client = setup_new_client(sock_fd,client_list);
            client_fd = this_client->socket_fd;
             if (client_fd < 0) {
                continue;
            }
            maxfd = size_compare(maxfd,client_fd);
            FD_SET(client_fd, &allfds);
            //printf("Accepted connect ID# %d\n",this_client->id);
            }
         for (int index = 0; index < MAX_CLIENTS; index++) {
            Client * this_client = client_list[index];
            if ( this_client != NULL && this_client->socket_fd > -1 && FD_ISSET(this_client->socket_fd, &listenfds)) {
                int thisfd = this_client->socket_fd;
            

                char command[BUFSIZE] = {'\0'};
                char *after = command;
                int nbytes;
                int where;
                this_client->buffer.inbuf = 0;
                this_client->buffer.consumed = BUFSIZE;
                //char * return_str = malloc(BUFSIZE * sizeof(char));
                while ((nbytes = read(thisfd, after, this_client->buffer.consumed)) > -1)
                {
                    this_client->buffer.inbuf += nbytes;
                    
                     if(nbytes == 0){ //  [TODO] delete info on disconnecting clients
                        FD_CLR(this_client->socket_fd, &allfds);
                        //free the list
                        //destroy contents
                        printf("[CLIENT %d] Connection closed\n",thisfd);
                        free(this_client);
                        client_list[index] = NULL;
                        close(thisfd);
                        break;
                    }
                     while ((where = find_network_newline(command, this_client->buffer.inbuf)) > 0)
                     { //complete command
                        command[where - 2] = '\0';
                        command[where - 1] = ' ';
                         //issue commands here=============================
                        int mode = get_mode(command,thisfd);
                        JobNode * job;
                        WatcherNode *wnode = NULL;
                        char log[BUFSIZE];
                        if(snprintf(log,BUFSIZE,"[CLIENT %d] %s\n",thisfd,command) > 0){
                            printf("%s",log);
                        }
                        int status;
                        for(int i = 0; i < MAX_JOBS; i++){
                            if(job_list[i] != NULL){
                                waitpid(job_list[i]->pid,&status,WNOHANG);
                                if(waitpid(job_list[i]->pid,&status,WNOHANG)){
                                    kill_job(job_list[i],job_list);
                                }
                            }
                        }
                        switch (mode) 
                        {
                            case 0://jobs
                            
                                print_active_jobs(job_list,thisfd);
                                break;
                            
                            case 1://run
                                job = create_jobnode(job_list,thisfd);
                                if(job != NULL){
                                    wnode = malloc(sizeof(WatcherNode));
                                    wnode->client_fd = thisfd;
                                    wnode->next = NULL;
                                    WatcherList  wlist;
                                    wlist.count = 1;
                                    wlist.first = wnode;
                                    job->watcher_list = wlist;
                                    start_job(command,job,thisfd,fd);
                                }
                                break;
                            case 2://kill
                                kill_command(command,job_list,thisfd);
                                break;
                            case 3://watch
                                break ;
                            default:
                                break;
                        }
                        
                        
                         //===========================================
                        this_client->buffer.inbuf -= where; // the length of the overflow
                        after = &(command[where]);
                        memmove(command, after, this_client->buffer.inbuf);
                     }
                    after = &(command[this_client->buffer.inbuf]);
                    this_client->buffer.consumed = BUFSIZE - this_client->buffer.inbuf;
                 }

                memset(command, 0, BUFSIZE);
                memset(command, 0, BUFSIZE);
             }
         }
    }

    for(int i = 0; i < MAX_JOBS; i++)
        {
            if (job_list[i] != NULL) {
                free(job_list[i]);
                }
                                
                                
        }
    for(int i = 0; i < MAX_CLIENTS; i++)
        {
            if (client_list[i] != NULL) {
                write(client_list[i]->socket_fd,"[SERVER] Shutting down\r\n",25);
                close(client_list[i]->socket_fd);
                free(client_list[i]);
            }
         }    
        free(client_list);
        free(job_list);
        close(sock_fd);
        close(fd[0]);
        close(fd[1]);
        exit(0);
    return 1;
}




 