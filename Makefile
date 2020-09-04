CC := g++ 
LDLIBS := -lpthread 
CFLAGS := -Wall -g -m64
APPS := sort

all: $(APPS)

sort: main.cpp sort.cpp factory.cpp
	$(CC) -std=c++11 ${CFLAGS} -o $@ $^ ${LDLIBS}

clean:
	rm -f *.o $(APPS)
