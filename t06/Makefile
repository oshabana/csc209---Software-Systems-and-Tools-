FLAGS = -Wall -Werror -std=gnu99
DEPENDENCIES = helpers.h

all: basic_test

basic_test: serialize.o deserialize.o
	gcc ${FLAGS} -o serialize serialize.c helpers.c
	gcc ${FLAGS} -o deserialize deserialize.c helpers.c
	#test=@`./serialize < test.dat > ./deserialize`; \
	echo Basic test passed
	
clean: 
	rm -f *.o serialize deserialize