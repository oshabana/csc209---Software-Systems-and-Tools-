#include <stdio.h>
// Add your system includes here.
#include <string.h>
#include <stdlib.h>
#include "ptree.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

// Defining the constants described in ptree.h
const unsigned int MAX_PATH_LENGTH = 1024;
//const char *PROC_ROOT="tests";

// If TEST is defined (see the Makefile), will look in the tests 
// directory for PIDs, instead of /proc.

#ifdef TEST
    const char *PROC_ROOT = "tests";
#else
    const char *PROC_ROOT = "/proc";
#endif


struct child_of * generate_struct_children(pid_t pid){

    char children_path[MAX_PATH_LENGTH + 1];
    struct child_of * root = malloc(sizeof(struct child_of ));
    struct stat buff1;
    FILE * children_file;
    root->name = pid;

    if(snprintf(children_path,MAX_PATH_LENGTH + 1,"%s/%d/task/%d/children",PROC_ROOT,pid,pid) < 0){
	    strcpy(root->children," ");
	    root->num_child = 0;
    	return root;
	}
    if(lstat(children_path,&buff1) < 0){
        strcpy(root->children," ");
        return root;
    }
    else{
        children_file = fopen(children_path,"r");
    }
    fgets(root->children,MAX_PATH_LENGTH + 1, children_file);
    return root;
}

int count_children(struct child_of * root, int * arr){

	int i = 0;
	char *p;
    int check = strtol(root->children,&p,10);
	while(check != 0){
		arr[i] = check;
		check = strtol(p,&p,10);
		if(arr[i] == 0){
			check = 0;
			i -= 1;
		}
			i++;
	}
	root->num_child = i;
	return i;
}

int generate_ptree(struct TreeNode ** root, pid_t pid) {
    // Here's a trick to generate the name of a file to open. Note 
    // that it uses the PROC_ROOT variable
    char procfile[MAX_PATH_LENGTH + 1];
    char exefile[MAX_PATH_LENGTH + 1];
    char cmdfile[MAX_PATH_LENGTH + 1];
    char childrenfile[MAX_PATH_LENGTH + 1];
    struct stat buff1;
    struct stat buff2;
    struct stat buff3;
    struct stat buff4;
    struct stat buff5;
   // struct stat buff6;
    char * cmd = malloc(sizeof(char) * 1024);
    FILE * cmd_file;
    int cmd_err = 0;

    if (snprintf(procfile, MAX_PATH_LENGTH + 1, "%s/%d",PROC_ROOT,pid) < 0) {                           
        fprintf(stderr, "snprintf failed to produce a filename for procfile\n");
        return 1;
    }

    if (snprintf(exefile, MAX_PATH_LENGTH + 1, "%s/%d/exe",PROC_ROOT,pid) < 0) {
        fprintf(stderr, "snprintf failed to produce a filename for procfile\n");
        return 1;
    }

    if (snprintf(cmdfile,MAX_PATH_LENGTH + 1,"%s/%d/cmdline",PROC_ROOT,pid) < 0){
	    fprintf(stderr, "snprintf failed to produce a filename for cmdline\n");
		return 1;
	}
    if(snprintf(childrenfile,MAX_PATH_LENGTH + 1,"%s/%d/task/%d/children",PROC_ROOT,pid,pid) < 0){
	    fprintf(stderr, "snprintf failed to produce a filename for children\n");
	}
    //checks for files
    if(lstat(procfile,&buff1) < 0){
       fprintf(stderr,"File of %d does not exist!\n",pid);
      // *root = NULL;
       return 1;
    }
    if(lstat(exefile,&buff2) < 0){
       fprintf(stderr,"File of exe for %d does not exist!\n",pid);
       return 1;
    }
    if(lstat(cmdfile,&buff5) < 0){
        cmd_err = -1;
        cmd = NULL;
        fprintf(stderr,"File cmdline for %d does not exist!\n",pid);
    }
 
    
//============================Variables====================================
    

    struct  child_of * children_struct = malloc(sizeof(struct child_of));
    int number_of_children = 0;
    int c_in_int[1000];
    children_struct = generate_struct_children(pid);
    number_of_children = count_children(children_struct,c_in_int);
    struct TreeNode * child = malloc(sizeof(struct TreeNode ) * number_of_children);
    struct TreeNode * child_ptr = malloc(sizeof(struct TreeNode));
    int check1 = 0;
    int check2 = 0;
    int check3 = 0;
    int check4 = 0;
//=========================================================================  
    if(*root != NULL && cmd_err == 0){
        cmd_file = fopen(cmdfile,"r");
        (*root)->name = malloc(sizeof(char) * MAX_PATH_LENGTH);
        fgets(cmd, 150, cmd_file);
        (*root)->name = cmd;
        (*root)->pid = pid;
    }
    else if (cmd_err == -1){
        (*root)->name = malloc(sizeof(char) * MAX_PATH_LENGTH);
        (*root)->name = cmd;
        (*root)->pid = pid;
        free(cmd);
        free((*root)->name);
    }
    if(number_of_children == 0){
        (*root)->child_procs = NULL;
    }
    else{//if it has children
        (*root)->child_procs = &child[0];
        for(int i = 0;i < number_of_children;i++){   
            snprintf(procfile, MAX_PATH_LENGTH + 1, "%s/%d",PROC_ROOT,c_in_int[i]);
            snprintf(exefile, MAX_PATH_LENGTH + 1, "%s/%d/exe",PROC_ROOT,c_in_int[i]);
            check1 = lstat(procfile,&buff3);
            check2 = lstat(exefile,&buff4);
            //printf("For pid: %d proc = %d and exe = %d\n",c_in_int[i],check1,check2);
            if(check1 == 0 && check2 == 0){
                //printf("%d exists and all is good in the world\n",c_in_int[i]);
                child_ptr = &child[i];
                generate_ptree(&child_ptr,c_in_int[i]);
            }
            else if((check1 != 0 || check2 != 0) && (number_of_children > i+1) && i == 0){
                (*root)->child_procs = &child[i+1];
            }
            else if((check1 != 0 || check2 != 0) && number_of_children > i+1 )
            {   
                child[i-1].next_sibling = &child[i+1];
                
            }
            
            else{
                //(*root)->child_procs = NULL;
                break;
                }
            if( number_of_children - 1 != i ){
                snprintf(procfile, MAX_PATH_LENGTH + 1, "%s/%d",PROC_ROOT,c_in_int[i+1]);
                snprintf(exefile, MAX_PATH_LENGTH + 1, "%s/%d/exe",PROC_ROOT,c_in_int[i+1]);
                check3 = lstat(procfile,&buff3);
                check4 = lstat(exefile,&buff4);
                //if(check1 == -1 || check2 == -1){
               // }
                if(check3 >= 0 && check4 >= 0){
                    child[i].next_sibling = &child[i+1];
                    }
             }
            else{
                child[i].next_sibling = NULL;
                //fprintf(stderr,"File of exe for %d does not exist, therefore its branch will be ignored\n",pid);
            }
            
         }
    
    }
     return 0;
     //Calling free() was very difficult here because it kept making the input radically incorrect or it simply wouldn't run
     //I guess due to the recursive nature of this function
}

void print_ptree(struct TreeNode *root, int max_depth) {
    // Here's a trick for remembering what depth (in the tree) you're at
    // and printing 2 * that many spaces at the beginning of the line.
    static int depth = 0;
    printf("%*s", depth * 2, "");
   
    if(root != NULL && root->name != NULL){
	    printf("%d: ",root->pid);
	    printf("%s\n",root->name);
	}
    else{
        printf("%d: ",root->pid);
	    printf("\n");

    }
    if(max_depth == 0){
  		 if(root->child_procs != NULL){
			depth++;
       		print_ptree(root->child_procs, 0);
            depth--;
			}
		 if (root->next_sibling != NULL){
      		print_ptree(root->next_sibling, 0);
			}
		}

	else if(max_depth > 0 ){
  		 if(root->child_procs != NULL && max_depth > depth+1 ){
			depth++;
       		print_ptree(root->child_procs, max_depth--); 
            max_depth++;
            depth--;
		    }
		 if (root->next_sibling != NULL ){
      		print_ptree(root->next_sibling, max_depth);
	        }
	    }
    
}
