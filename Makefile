CXX=clang++
CXXFLAGS=-Wall -Wextra -std=c++17

test: tests/main.cpp
	$(CXX) $(CXXFLAGS) -o bin/test tests/main.cpp
	./bin/test

install:
	install -m 0644 src/solana.h /usr/local/include
