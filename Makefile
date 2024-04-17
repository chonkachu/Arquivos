CFLAGS=-O2 -Wall -Iinclude/
CC=gcc
LIBOBJS=$(patsubst %.c,%.o,$(wildcard include/*.c))
OBJSLOCAL=$(patsubst include/%.o, %.o, $(LIBOBJS))
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

all: $(OBJS) $(LIBOBJS)
	$(CC) $< $(OBJSLOCAL) -o main
%.o: %.c
	$(CC) -c $< $(CFLAGS)
run: all
	./main
clean:
	rm ./main
	rm *.o
clean_input:
	rm *.in
	rm *.out
