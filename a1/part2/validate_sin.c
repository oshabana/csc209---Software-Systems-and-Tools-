#include <stdio.h>
#include <stdlib.h>
//#include "sin_helpers.c"

int populate_array(int, int *);
int check_sin(int *);
int disect_greater_than(int);

int main(int argc, char **argv) {
	if (argc != 2){ return 1;}

    // TODO: Verify that command line arguments are valid.
	int * array = malloc(sizeof(int)*50);
	int size_nine = 1;
	int valid = 1;
	int sin_num = strtol(argv[1],NULL,10);
    // TODO: Parse arguments and then call the two helpers in sin_helpers.c
    // to verify the SIN given as a command line argument.
 	if (argc == 2){ //does it have 2 arguments

		size_nine = populate_array(sin_num,array);

		if(size_nine == 0){		//is the SIN size 9
			valid = check_sin(array);

			if (valid == 0){// is it a valid SIN given that it is 9 long
				printf("Valid SIN\n");

				return 0;
			}
		}
	printf("Invalid SIN\n");
	} 
    	return 1;
}
