CXX=clang++
CXX=g++
CXXFLAGS=-g -Wall -Wextra -std=c++17 -O2

test: tests/main.cpp
	$(CXX) $(CXXFLAGS) -o bin/test tests/main.cpp -lcrypto -lssl
	./bin/test

install:
	install -m 0644 src/solana.h /usr/local/include
