#include <string>
#include <map>
#include <vector>
#include "symTable.h"
#include "error.h"

using namespace std;

SymTable::SymTable() {
    currentScope = 0;
}

void SymTable::openScope() {
    ++currentScope;
}

void SymTable::closeScope() {
    table.erase(currentScope);
    --currentScope;
}

Error SymTable::enterSymbol(string name, string type) {
    Error noError;
    noError.type = Error::ErrorType::VOID;
    noError.id = Error::ErrorID::VOID;
    
    // symbol name already in use
    if (declaredLocally(name)) {
        // in use in current scope
        Error revar;
        revar.type = Error::ErrorType::WARN;
        revar.id = Error::ErrorID::REVAR;
        return revar;
    } else {
        // if not in current scope
        vector<string> newData;
        newData.push_back(name);
        newData.push_back(type);
        newData.push_back("NO");
        table[currentScope].push_back(newData);
        return noError;
    }

    return noError;
}

vector<string> SymTable::getSymbol(string name) {
    for (int scope = currentScope; scope >= 0; scope--) {
        for (vector<string> symbol : table[scope]) {
            if (symbol[NAME] == name) {
                return symbol;
            }
        }
    }

    vector<string> noSymbol;
    noSymbol.push_back("dne");
    return noSymbol;
}

bool SymTable::declaredLocally(string name) {
    for (vector<string> sym : table[currentScope]) {
        if (sym[NAME] == name) {
            return true;
        }
    }

    return false;
}

void SymTable::init(string name) {
    for (int scope = currentScope; scope >= 0; scope--) {
        for (vector<string>& symbol : table[scope]) {
            if (symbol[NAME] == name) {
                symbol[INITIALIZED] = "YES";
                scope = -1;
                break;
            }
        }
    }
}

bool SymTable::isInit(string name) {
    vector<string> symbol = getSymbol(name);
    
    if (symbol.size() == 1) return false; // DNE case for getSymbol
    return symbol[INITIALIZED] == "YES";
}

int SymTable::getCurrentScope() {return currentScope;}

std::map<unsigned int, std::vector<std::vector<std::string>>> SymTable::getTable() {return table;}