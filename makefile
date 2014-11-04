CC=gcc
CFLAGS=-c -std=c11 -Wall -g
LDFLAGS=
SOURCES=$(wildcard *.c)
HEADERS=$(wildcard *.h)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))
EXECUTABLE=geng

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -f $(OBJECTS) $(EXECUTABLE)

debug: $(SOURCES) $(EXECUTABLE)
	gdb ./$(EXECUTABLE)

valgrind: $(SOURCES) $(EXECUTABLE)
	valgrind ./$(EXECUTABLE)

format: 
	astyle -A8 -t $(SOURCES) $(HEADERS)
	find -iname '*.[chCH].orig' -delete
