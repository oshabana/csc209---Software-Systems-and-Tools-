#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char **argv) {

	int i;
	int n;
	int num_kids;
	int p = getpid(); //will be the first instance so the parents pid

	if (argc != 2) {
		fprintf(stderr, "Usage: parentcreates <numkids>\n");
		exit(1);
	}

	num_kids = strtol(argv[1], NULL, 10);

	for (i = 0; i < num_kids; i++) {
		if(getpid() == p){ // if current pid is the parents
			n = fork();    //make a child
	 		printf("pid = %d, ppid = %d, i = %d\n", getpid(), getppid(), i);
		}
		if (n < 0) {
			perror("fork");
			exit(1);
		}
		wait(NULL);
	}

	return 0;
}
