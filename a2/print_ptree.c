#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ptree.h"

int main(int argc, char **argv) {
    // TODO: Update error checking and add support for the optional -d flag
    // printf("Usage:\n\tptree [-d N] PID\n");
	if(argc!=2 && argc!=4){
		//printf("%d\n",argc);
		fprintf(stderr,"Usage:\n\tptree [-d N] PID\n");	
		return 1;
	}

	int depth,pid = 0;
 	char * modifer = "-d";

	
	if(strcmp(argv[1],modifer) == 0){
		if (argc == 4)
		  depth = strtol(argv[2],NULL,10);
		else{
			fprintf(stderr,"Usage:\n\tptree [-d N] PID\n");	
			return 1;
			}
	}
    // NOTE: This only works if no -d option is provided and does not
    // error check the provided argument or generate_ptree. Fix this!
    struct TreeNode * root = malloc(sizeof(struct TreeNode));

  if(argc == 2 || argc==4){
			if(argc == 2){
				pid = strtol(argv[1], NULL, 10);
  	 		if(generate_ptree(&root, pid) == 0)
   	 			print_ptree(root, depth);
				else
				{
					fprintf(stderr,"Generating a tree for this PID failed\n");	
					return 2;
				}
				
	  		return 0;
	 }
			else if (argc == 4){
				pid = strtol(argv[3], NULL, 10);
  	 	  if(generate_ptree(&root, pid) == 0)
   	 			print_ptree(root, depth);
				else
				{
					fprintf(stderr,"Generating a tree for this PID failed\n");	
					return 2;
				}
	  		return 0;

		}
   
	}
    return 2;
}

