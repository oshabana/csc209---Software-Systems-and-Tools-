#include <stdio.h>
#include <string.h>
#include "eratosthenes.h"
#include <stdlib.h>
#define MAXSIZE 15000

int filter(int n, int readfd, int writefd) {
    char * list = malloc(sizeof(char) * MAXSIZE);
    char * cmp = malloc(sizeof(char) * 10);
    char * str_n = malloc(sizeof(char) * 10);

    read(readfd,list,sizeof(char) * MAXSIZE);
    snprintf(str_n,sizeof(char)* 10, " %d ",n);
    int size_str= strlen(str_n);
    
    if((cmp = strstr(list,str_n)) != NULL){
        for(int i = 0; i < size_str; i++){
            cmp[i] = ' ';
        }
        //strcat(list,cmp);
        write(writefd,list,sizeof(char) * MAXSIZE);
        
    }

    free(str_n);
    free(cmp);
    //free(list);
    return 0;
}


