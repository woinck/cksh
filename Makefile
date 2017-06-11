TARGET = build/yaush
CFILES = $(wildcard *.c)
OBJS = $(CFILES:.c=.o)
CC ?= clang 
#@CFLAGS = -Wall -DDEBUG -g3
CFLAGS = -Wall -g3

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -lreadline -o $@
	rm $(OBJS)

$(OBJS): $(CFILES)
	$(CC) $(CFLAGS) -c $(@:.o=.c) -o $@ 

clean:
	rm $(TARGET)
