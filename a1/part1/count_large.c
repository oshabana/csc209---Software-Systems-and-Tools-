#include <stdio.h>
#include <stdlib.h>



// TODO: Implement a helper named check_permissions that matches the prototype below.


int check_permissions(char * given, char * required){
	for (int i = 0; i < 10; i++){
		if(given[0] == 'd'){
			return 1;
		}
		if(required[i] != '-'){
			if(required[i] != given[i+1]){
//				printf("on run %d from required: %c is not the same as the given's: %c\n",i+1,required[i],given[i]);
				return 1;

			}
		}
	}

	return 0;
}

int main(int argc, char** argv) {
    if (!(argc == 2 || argc == 3)) {
        fprintf(stderr, "USAGE:\n\tcount_large size [permissions]\n");
        return 1;
    }

	int total;
	char  total_word[9];


	char  given_permissions[10];
	int random_int;
	char  usr[15];
	char  student[10];
	int  size_file;
	char  month[12];
	char date[9];
	char  time[10];
	char  file_name[32];

	int comparison_size = strtol(argv[1],NULL,10);
	char * modifier = argv[2];
	int size_counter = 0;

	scanf("%s %d",total_word,&total);


	if (argc == 3){

		while(scanf("%s %d %s %s %d %s %s %s %s",given_permissions,&random_int,usr,student,&size_file,month,date,time,file_name) != EOF)
		{
//			scanf("%s %d %s %s %d %s %s %s %s",line,&random_int,usr,student,&size_file,month,date,time,file_name);
//			printf("%s %d %s %s %d %s %s %s %s \n",line,random_int,usr,student,size_file,month,date,time,file_name);
			if (comparison_size < size_file && given_permissions[0] != 'd'){

				if(check_permissions(given_permissions,modifier) == 0){

					size_counter++;
				}
					}

		}
	printf("%d\n",size_counter);

	}

	else if(argc == 2){
		while(scanf("%s %d %s %s %d %s %s %s %s",given_permissions,&random_int,usr,student,&size_file,month,date,time,file_name) != EOF)
		{
//			printf("%s %d %s %s %d %s %s %s %s \n",line,random_int,usr,student,size_file,month,date,time,file_name);
			if (comparison_size < size_file && given_permissions[0] != 'd'){
				size_counter++;
					}
	

		}
	printf("%d\n",size_counter);
	
	}

    return 0;
}
