CC = gcc
OBJS = myset.o analyze.o set.o
EXEC = myset
DEBUG_FLAG = -g
COMP_FLAG = -Wall -ansi -pedantic $(DEBUG_FLAG)

$(EXEC): $(OBJS)
	$(CC) $(DEBUG_FLAG) $(OBJS) -o $@

myset.o: myset.c analyze.h set.h
	$(CC) -c $(COMP_FLAG) $*.c

analyze.o: analyze.c analyze.h set.h
	$(CC) -c $(COMP_FLAG) $*.c

set.o: set.c set.h
	$(CC) -c $(COMP_FLAG) $*.c

clean:
	rm -f $(OBJS)

