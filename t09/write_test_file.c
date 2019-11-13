#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
int main(int argc, char ** argv) {
  
    char filename[1000];
    sprintf(filename,"%s",argv[1]);
    FILE * create = fopen(filename,"w");
    fclose(create);
    FILE * num = fopen(filename,"r+b");

    char bin[50];

    for(int i = 0; i < 100; i++)
    {
      
        sprintf(bin,"0x%ld ",random() % 100);
        fputs(bin,num);
        
    }
    


    return 0;
}