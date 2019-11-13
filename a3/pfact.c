#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <math.h>

#include "eratosthenes.h"
#define MAXSIZE 15000

char * make_list(int n){
    char * list = malloc(sizeof(char) * MAXSIZE);
    char buf[10];
    for(int i = 2;i <= n; i++){
            snprintf(buf,sizeof(char)*10," %d",i);
            strncat(list,buf,MAXSIZE);
        }
    return list;
}
void add_factors(int factor,int readfd,int writefd){
    char buf[10];
    snprintf(buf,sizeof(char)*10," %d ",factor);
    write(writefd,buf,sizeof(char) * 10);
}

int is_in_list(int n,char * list){
    char * cmp;
    char * str_n = malloc(sizeof(char) * 10);
    snprintf(str_n,sizeof(char)* 10, " %d",n);
   
    if((cmp = strstr(list,str_n)) != NULL){
        free(str_n);
        return 0;
    }
    free(str_n);
    return 1;
}
int main(int argc, char *argv[]) {
    // Turning off sigpipe
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal");
        exit(1);
    }
    int target;
    if(argc == 2){
        if ((target = strtol(argv[1],NULL,10)) >= 0){
            ;
        }
        else
        {
            fprintf(stderr,"Invalid input!");
            exit(1);
        }
    }
    else
    {   
        fprintf(stderr,"Invalid input!");
        exit(1);
    }

    //double sqrt_n = sqrt(target);
    int fd1[2];
    int fd2[2];
    int * fds2 = fd2;
    char * list =  make_list(target);
    int pid = 1;
    

    pipe(fd1);
    pipe(fd2);

    int factor_list[100];
    int prime_list[15];
    int prime_factors = 0;
    int factors = 0;
    int filters = 0;

    write(fd1[1],list,sizeof(char) * MAXSIZE);
    
      for(int i = 2; i < target; i++)
        { 
          if( is_in_list(i,list) == 0 && pid > 0){
            filters++;
          }
        for(int j = 1; i*j < target ; j++)
        {
            if( is_in_list(i*j,list) == 0) 
            {
                if((target % (i*j))  == 0 && j == 1)
                {
                    if(target / (i*j) == i*j){
                        factor_list[factors] = i*j;
                        factors++;
                    }
                    factor_list[factors] = i*j;
                    factors++;
                    wait(0);
                    break;
                    
                }
                else
                {
                    pid = make_stage(i*j,fd1[0],&fds2);
                    
                     if (pid > 0) 
                     {
                        wait(0);
                        exit(0);
                    }
                    
                }
                read(fd2[0],list,sizeof(char) * MAXSIZE);
                write(fd1[1],list,sizeof(char) * MAXSIZE);
                //printf("%s   %d\n",list,i*j);
            }
            
            }
        }
            
    for(int k = 0; k < factors; k++)
    {
        if (is_in_list(factor_list[k],list) == 0) {
            prime_list[prime_factors] = factor_list[k];
            prime_factors++;
           
            if (prime_factors == 2) {
                printf("%d %d %d\n",target,prime_list[0],prime_list[1]);
                printf("Number of filters = %d\n",filters);
                return 0;
                exit(0);
                
            }
            
        }
        
    }
    if(factors == 0 )
    {
        printf("%d is prime\nNumber of filters = %d\n",target,filters);
        return 0;
        exit(0);
    }
    else
    {
        printf("%d is not the product of two primes\nNumber of filters = %d\n",target,filters);
        return 0;
        exit(0);
    }
    close(fd1[0]);
    close(fd2[0]);
    close(fd1[1]);
    close(fd2[1]);
    
    exit(0);
    return 0;
}
