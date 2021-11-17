CC = g++ 
CFLAGS = -Wall -Wno-deprecated 
all: implementation_1 implementation_2
implementation_1: 
	$(CC) $(CFLAGS) sol1.c -o implementation_1 -pthread

implementation_2: 
	$(CC) $(CFLAGS) sol2.c -o implementation_2 -pthread
clean:
	rm *.o implementation_1 , implementation_2