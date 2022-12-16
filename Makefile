CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=src/solana.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=lib/libsolana.a

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	ar rcs $@ $(OBJECTS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)

install:
	install -m 0644 src/solana.h /usr/local/include
	install -m 0644 lib/libsolana.a /usr/local/lib
