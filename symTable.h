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

private:
    // name -  type, scope, opt. replacement id
    std::map<std::string, std::vector<std::string>> table;
    int currentScope;
};

#endif /*SYMTABLE_H*/