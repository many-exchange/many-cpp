CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=libsolana/solana.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=libsolana.a

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	ar rcs $@ $(OBJECTS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)

install:
	install -m 0644 libsolana/solana.h /usr/local/include
	install -m 0644 libsolana.a /usr/local/lib
