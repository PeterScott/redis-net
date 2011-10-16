CC = gcc 
CFLAGS = -Wall -g

OBJS = ae.o zmalloc.o anet.o main.o

all :x

x : $(OBJS)
	$(CC) $(CFLAGS) -o main $^

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $^


clean :
	rm -rf *.o main



