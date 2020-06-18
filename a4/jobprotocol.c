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

#include "socket.h"
#include "jobprotocol.h"
#define QUEUE_LENGTH 5
#define MAX_CLIENTS 20
#ifndef JOBS_DIR
    #define JOBS_DIR "jobs/"
#endif
#define JOB_NUM 5
#ifndef MAX_JOBS
    #define MAX_JOBS 32
#endif

char available_jobs[5][10] = {"jobs","run","kill","watch"};




// TODO: Use this file for helper functions (especially those you want available
// to both executables.

/* Example: Something like the function below might be useful

   // Find and return the location of the first newline character in a string
   // First argument is a string, second argument is the length of the string
   int find_newline(const char *buf, int len);
  
*/
//==================================CLIENT=====================================
void remove_nullterm(char * str){
    for(int i = 0; i < strlen(str); i++)
        {
            if (str[i]== '\0'){
                str[i]= ' ';
            }
        }
}

void add_networknewline(char * pid_list){
    strncat(pid_list,"\r",1);
    strncat(pid_list,"\n",1);
}

int find_network_newline(const char *name, int inbuf) {
    for(int i = 0; i < inbuf; i++)
    {
        if (name[i] == '\r') {
            if (name[i+1] == '\n') {
                return (i + 2);
            }
        }
    }
    return -1;
}

void remove_networknewline(char * str){
    for(int i =0;i < strlen(str);i++){
        if(str[i] == '\r'){
            str[i] = '\0';
            
        }
        else if (str[i] == '\n') {
            str[i] = '\0';
        }
        
    }
}


/* makes a list of NULLed Clients*/
Client ** make_client_list(int max){
    Client ** client_list = malloc(sizeof(Client *) * max);
    for(int i = 0; i < max; i++)
    {
        client_list[i] = NULL;
    }
    
    return client_list;
}

/*
Returns free space index for client. Returns -1 if nothing in the list equals
NULL
*/
int find_spot(Client ** client_list){
    for(int spot = 0; spot < MAX_CLIENTS; spot++)
    {
        if(client_list[spot] == NULL){
            return spot; 
        }
    }
    printf("Failed to find a spot\n");
    return -1;
}

/*
Sets up new client and intializes its paramaters
*/
Client * setup_new_client(int listenfd, Client ** client_list){
    int spot = find_spot(client_list);
    int fd;
    if (spot == -1) {
        fprintf(stderr,"Too many clients running!\n");
        return NULL;
    }
    Client * new_client = malloc(sizeof(Client));
    fd = accept_connection(listenfd);
    if(fd > -1){
        client_list[spot] = new_client; 
        new_client->socket_fd = fd;
        new_client->buffer.consumed = 0;
        new_client->buffer.inbuf = 0;
        new_client->id = spot;
        return new_client;
    }
    printf("Failed to make client\n");
    return NULL;

}

int size_compare(int fd1,int fd2){
    if(fd1 > fd2){
        return fd1;
    }
    else
    {
        return fd2;
    }
    
}

void move_char(char * dest,char * source,int max){
    for(int i = 0; i < max; i++)
    {
        dest[i] = source[i];
    }
    
}

//====================PARSING COMMANDS===================================
int existing_job_check(char * jobname){
    char path[BUFSIZE - 5];
    struct stat buf1;
    snprintf(path, BUFSIZE - 5, "%s/%s", JOBS_DIR, jobname);
    
    if(stat(path,&buf1) < 0){
        printf("[SERVER] Invalid command: %s",jobname);
        return -1;
    }
    return 0;
}

/*checks if command is valid i.e. is in the list of jobs defined above*/
int set_mode(char * name){
    int mode = -1;

    if(strlen(name) < BUFSIZE){
        for(int i = 0; i < JOB_NUM; i++)
        {
            if(strcmp(available_jobs[i],name) == 0){
                mode = i;
                return mode;
            }
        }
        printf("[SERVER] Invalid command: %s\n",name);

    }
    return mode;
}

//============================JOBS==========================================
/*JobNode * setup_job(char * command,JobNode ** job_list){
    char command_name[10];
    char jobname[BUFSIZE];
    JobNode * job = NULL;
    if(sscanf(command,"%s %s",command_name,jobname) == EOF){
        perror("EOF error");
    }
    //printf("%s",command_name);
    //printf("%s",jobname);
    
    int mode = set_mode(command_name);
    if(mode < 0){ // will never get in here becuase the client forbids this
        perror("Invalid command type");
        return job;
    }
    if(mode == 1){
        if(existing_job_check(jobname) == 0){
            }
         else
         {
               return job;
        }
    }
    job = create_jobnode(job_list);
    job->mode = mode;
    return job;
    
}
*/
int get_mode(char * command,int clientfd){
    char command_name[10];
    char jobname[BUFSIZE];
    
    if(sscanf(command,"%s %s",command_name,jobname) < 0){
        perror("EOF error");
    }
    
    
    int mode = set_mode(command_name);
    return mode;
}


JobNode ** create_joblist(int max){
     JobNode ** job_list = malloc(sizeof(JobNode *) * max);
    for(int i = 0; i < max; i++)
    {
        job_list[i] = NULL;
    }
    
    return job_list;
    }

int check_job_availability(JobNode ** list,int clientfd){
    for(int spot = 0; spot < MAX_JOBS; spot++)
    {
        if(list[spot] == NULL){
            return spot; 
        }
    }
    char msg[50] = "[SERVER] MAXJOBS exceeded\r\n";
    write(clientfd,msg,50);
    return -1;
}


JobNode * create_jobnode(JobNode ** list,int clientfd){
    int spot = check_job_availability(list,clientfd);
    if(spot == -1){
        perror("");
        return NULL;
    }
    JobNode * new_node = malloc(sizeof(JobNode));
    new_node->dead = 0;
    new_node->spot = spot;
    new_node->pid = -1;
    list[spot] = new_node;
    return list[spot];
}


void start_job(char * command, JobNode * node, int clientfd, int * fd){
    int pid = fork();
    if(pid < 0){
        perror("fork");
        exit(1);
    }
    if(pid > 0)
    {
        node->pid = pid;
        return;
    }
    else if(pid == 0){
        struct stat buf1;
        char path[BUFSIZE];
        char command_name[BUFSIZE];
        char jobname[BUFSIZE];
        char args[BUFSIZE];
        if(sscanf(command,"%s %s %s",command_name,jobname,args) == EOF)
        {
            perror("sscanf");
            exit(1);
        }
        if(snprintf(path,(strlen(JOBS_DIR)+strlen(jobname)+1),"%s%s",JOBS_DIR,jobname) < 0){
            perror("snprintf\n");
            exit(1);
        }
        remove_networknewline(path);
        if(stat(path,&buf1) < 0){
            printf("[SERVER] Invalid command: %s\n",jobname);
            exit(1);
            }
        if(strlen(command) > BUFSIZE){
            dup2(clientfd,STDERR_FILENO);
            fprintf(stderr,"*(SERVER)* Buffer from job %d is full. Aborting job.\n",pid);
            exit(1);
        }
        printf("[SERVER] Job %d created\n",getpid());
        //put more intitalizers later!!
        dup2(clientfd,STDOUT_FILENO);
        dup2(clientfd,STDERR_FILENO);
        execl(path,args,NULL);
        perror("exec");
        exit(1);
    }
}


//UNTESTED
void kill_watchers(WatcherNode * wn){
    //send signal handler to process!
    if (wn->next == NULL) {
        free(wn->next);
    }
    else if(wn->next != NULL){
        kill_watchers(wn->next);
        free(wn->next);
    }
}

void kill_pid(int pid){
    if(kill(pid,SIGKILL) == 0)
    {
        
        return;
    }
    perror("kill");
}

void kill_job(JobNode * job,JobNode ** job_list){
    if(job != NULL){
        kill_watchers(job->watcher_list.first);
        job_list[job->spot] = NULL;
        kill_pid(job->pid);
        free(job);
    }
}

void kill_command(char * command, JobNode ** list,int clientfd){
        char command_name[BUFSIZE];
        char jobname[BUFSIZE];
        char args[BUFSIZE];

        if(sscanf(command,"%s %s %s",command_name,jobname,args) == EOF){
            perror("EOF error");
            
        }
        int pid = strtol(jobname,NULL,10);
        int p_index = pid_to_job(pid,list);
        
        if(p_index != -1){
            kill_job(list[p_index],list);
        }
        else
        {
            char msg[50];
            snprintf(msg,49,"Job %d not found\r\n",pid);
            write(clientfd,msg,49);
        }
        
}


void print_active_jobs(JobNode ** list,int clientfd){
    char pid_list[BUFSIZE] = {'\0'};
    char buf[15];
    strncat(pid_list, "[SERVER]", sizeof(char) * 10);
    int active_jobs = 0;
    for(int i = 0; i < MAX_JOBS; i++)
    {
        if((list[i]) != NULL ){
            snprintf(buf,sizeof(char) * 15," %d", (list[i])->pid);
            int current = strlen(pid_list) + strlen(buf);
            active_jobs++;
            if(current < BUFSIZE){
                strncat(pid_list,buf,15);
            }   
        }
        
    }
    
    if(active_jobs == 0){
       char msg[45] = "[SERVER] No currently running jobs\r\n";
       remove_nullterm(pid_list);
        write(clientfd,msg,45);
    }
    else
    {
        add_networknewline(pid_list);
        remove_nullterm(pid_list);
        write(clientfd,pid_list,strlen(pid_list)+1);
    }
}

int pid_to_job(int pid,JobNode ** list){

    for(int i = 0; i < MAX_JOBS; i++)
    {
        if (list[i] != NULL) {
            if (list[i]->pid == pid) {
                return i;
            }
            
        }
        
    }
    return -1;
}
int write_to(Buffer * jbuf){
    return -1;
}

