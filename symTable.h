#include <string>
#include <map>
#include <vector>
#include "error.h"

#ifndef SYMTABLE_H
#define SYMTABLE_H

class SymTable {
public:
    SymTable();

    void openScope();
    void closeScope();
    Error enterSymbol(std::string name, std::string type);
    std::vector<std::string> getSymbol(std::string name);
    bool declaredLocally(std::string name);
    void init(std::string name);
    bool isInit(std::string name);

    int getCurrentScope();
    std::map<unsigned int, std::vector<std::vector<std::string>>> getTable();

    static const unsigned int NAME = 0;
    static const unsigned int TYPE = 1;
    static const unsigned int INITIALIZED = 2;

private:

    // scope -  name, type, init
    std::map<unsigned int, std::vector<std::vector<std::string>>> table;
    unsigned int currentScope;
};

#endif /*SYMTABLE_H*/