CC      := g++
CFLAGS  := -std=c++17
LDFLAGS :=

debug: CFLAGS += -g -O0
debug: all

all: remove-audiotracks

remove-audiotracks: main.cpp
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f *.out *.o *.txt *.gch *.webm *.mkv
