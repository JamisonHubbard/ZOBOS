CXX=g++
CXXFLAGS=-Wall -Wpedantic -I. --std=c++17 -g
DEPS=grammar.o abstracttree.o node.o rule.o symTable.o

ZOBOS: $(DEPS) main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm *.o ZOBOS*
