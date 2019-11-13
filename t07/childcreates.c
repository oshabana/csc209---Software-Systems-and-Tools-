#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char **argv) {

	int i;
	int n;
	int num_kids;
	int parent_id = getpid();
	if (argc != 2) {
		fprintf(stderr, "Usage: childcreates <numkids>\n");
		exit(1);
	}

	num_kids = strtol(argv[1], NULL, 10);

	for (i = 0; i < num_kids; i++) {
		if(i == 0 && parent_id == getpid()){
			n = fork();
	 		printf("pid = %d, ppid = %d, i = %d\n", getpid(), getppid(), i);

		}
		else if (parent_id == getppid()){
			n = fork();
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
