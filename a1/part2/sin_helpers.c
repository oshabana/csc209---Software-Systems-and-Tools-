


// TODO: Implement populate_array
/*
 * Convert a 9 digit int to a 9 element int array.
 */
#include <stdio.h>


int populate_array(int sin, int *sin_array) {
	int i = 0;
	int reversed_sin = 0;
	
	while(sin > 0 ){
		reversed_sin = 10*(reversed_sin);	
		reversed_sin = reversed_sin + (sin % 10);
		sin = sin / 10;		
	}

	while (reversed_sin > 0){
		sin_array[i] = reversed_sin % 10;
		reversed_sin = reversed_sin / 10;
		i++;
	}

	if (i == 9){
		return 0;
	
	}
	return 1;
}

// TODO: Implement check_sin
/* 
 * Return 0 (true) iff the given sin_array is a valid SIN.
 */
int disect_greater_than(int i){
	int sum = 0;

	if (i >= 10){
		while(i != 0){
			sum = sum + i % 10;
			i = i / 10;
		}
		
		return sum;
	}
	return i;

	}

int check_sin(int * sin_array) {
	int i = 0;
	int sum = 0;
	int test_num = 0;
	int validation[9]={1,2,1,2,1,2,1,2,1};
	for (i=0;i<9;i++){
		test_num = disect_greater_than(validation[i] * sin_array[i]);
		sum = sum + test_num;

			}
	if(sum % 10 == 0){
	   return 0;
			}
	return 1;
}





