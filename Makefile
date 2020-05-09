CXX=g++
CXXFLAGS=-Wall -Wpedantic -I.
DEPS=grammar.o node.o rule.o symTable.o

ZOBOS: $(DEPS) main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm *.o ZOBOS*
