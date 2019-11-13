#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>


// Message to print in the signal handling function. 
#define MESSAGE "%ld reads were done in %ld seconds.\n"

/* Global variables to store number of read operations and seconds elapsed.
 * These have to be global so that signal handler to be written will have
 * access.
 */
long  num_reads = 0, seconds;

void handler(int code){

    fprintf(stderr,MESSAGE,num_reads,seconds/10000);
    exit(0);
}
int main(int argc, char ** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: time_reads s filename\n");
        exit(1);
    }
    int num;
    char mem[10000];
    struct sigaction newact;
    FILE *fp;
    if ((fp = fopen(argv[2], "r+")) == NULL) {    // Read+Write for later ...
      perror("fopen");
      exit(1);
    }
    newact.sa_handler = handler;
    newact.sa_flags = 0;
    sigemptyset(&newact.sa_mask);
    sigaction(SIGPROF,&newact,NULL);
    memset (&newact, 0, sizeof (newact));
    seconds = strtol(argv[1], NULL, 10) * 10000; //1000 milliseconds in a second
    
    
   
    struct itimerval itimer;
    itimer.it_value.tv_usec = seconds; 
    itimer.it_value.tv_sec = 0;
    itimer.it_interval.tv_sec = 0;
    itimer.it_interval.tv_usec = seconds;

 setitimer (ITIMER_PROF , &itimer, NULL);
    for (;;) {

      fseek(fp,random() % 100,SEEK_SET);
      fgets(mem,10000,fp);
      //printf("%s\n",mem);
      num = strtol(mem,NULL,16);
      printf("%d\n",num);
      num_reads++;

    }

    return 1;  //something is wrong if we ever get here!
}

