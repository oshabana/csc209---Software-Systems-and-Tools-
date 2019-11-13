



#include <stdio.h>
#include <stdlib.h>

int main(){
	int i = 0;
	char *numbers = malloc(10*sizeof(char));	
	int mod;

	while(i > -1){
		scanf("%s %d",numbers,&mod);
		if (mod == 0 ){
			printf("%s\n",numbers);
		}
		else if (mod >= 1 && mod <= 9){	
			printf("%c\n",numbers[mod]);
		}				
	} 	
	return 0 ;
}
