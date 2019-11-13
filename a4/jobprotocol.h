#ifndef __JOB_PROTOCOL_H__
#define __JOB_PROTOCOL_H__
#ifndef PORT
  #define PORT 52765 //55555
#endif

#ifndef MAX_JOBS
    #define MAX_JOBS 32
#endif

// No paths or lines may be larger than the BUFSIZE below
#define BUFSIZE 256
#define CMD_INVALID -1
#define PIPE_READ 0
#define PIPE_WRITE 1
#define MAX_CLIENTS 20
// TODO: Add any extern variable declarations or struct declarations needed.


struct job_buffer {
	char buf[BUFSIZE];
	int consumed;
	int inbuf;
};
typedef struct job_buffer Buffer;

struct client {
	int socket_fd;
	int id;
	int pid;
	struct job_buffer buffer;
};
typedef struct client Client;

struct watcher_node {
	int client_fd;
	struct watcher_node *next;
};
typedef struct watcher_node WatcherNode;

struct watcher_list {
	struct watcher_node* first;
	int count;
};
typedef struct watcher_list WatcherList;
struct job_node {
	int pid;
	int spot;
	int stdout_fd;
	int stderr_fd;
	int dead;
	int wait_status;
	int mode;
	struct job_buffer stdout_buffer;
	struct job_buffer stderr_buffer;
	struct watcher_list watcher_list;
//	struct job_node* next;
};
typedef struct job_node JobNode;

//======================functions===============
int find_network_newline(const char *buf, int inbuf);
Client ** make_client_list(int max);
int find_spot(Client ** client_list);
int set_mode(char * name);
Client * setup_new_client(int listenfd, Client ** clients);
int size_compare(int fd1,int fd2);
void remove_networknewline(char * str);

void move_char(char * dest,char * source, int max);
int existing_job_check(char * jobname);
JobNode ** create_joblist(int max);
int check_job_availability(JobNode ** list,int clientfd);
JobNode * create_jobnode(JobNode ** list,int clientfd);
void start_job(char * command, JobNode * node, int childfd,int * fd);
JobNode * setup_job(char * command,JobNode ** job_list);
void print_active_jobs(JobNode ** list,int clientfd);
int get_mode(char * command,int clientfd);
void kill_job(JobNode * job,JobNode ** job_list);
void kill_watchers(WatcherNode * wn);
int pid_to_job(int pid,JobNode ** list);
void kill_pid(int pid);
void kill_command(char * command, JobNode ** list,int clientfd);

void add_networknewline(char * pid_list);
void child_handler();
int write_to(Buffer * jbuf);
void remove_nullterm(char * str);

#endif