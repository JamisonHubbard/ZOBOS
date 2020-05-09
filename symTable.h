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
    std::map<std::string, std::vector<std::string>> getTable();

private:
    // name -  type, scope, opt. replacement id, init
    std::map<std::string, std::vector<std::string>> table;
    int currentScope;
};

#endif /*SYMTABLE_H*/