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
    --currentScope;

    vector<string> currentVars;
    for (pair<string, vector<string>> symbol : table) {
        if (symbol.second[1] == to_string(currentScope)) {
            currentVars.push_back(symbol.first);
        }
    }
    for (string id : currentVars) {
        if (table[id][2] != "") {
            vector<string> oldData = table[table[id][2]];
            table[id] = oldData;
            map<string, vector<string>>::iterator mit = table.find(table[id][2]);
            table.erase(mit);
        }
        else {
            map<string, vector<string>>::iterator mit = table.find(id);
            table.erase(mit);
        }
    }
}

Error SymTable::enterSymbol(string name, string type) {
    Error noError;
    noError.type = Error::ErrorType::VOID;
    noError.id = Error::ErrorID::VOID;
    
    // symbol name already in use
    if (table.find(name) != table.end()) {
        // in use in current scope
        if (table[name][1] == to_string(currentScope)) {
            Error revar;
            revar.type = Error::ErrorType::WARN;
            revar.id = Error::ErrorID::REVAR;
            return revar;
        }
        // if not in current scope
        string newName = name + "XX";
        table[newName] = table[name];
        vector<string> newData;
        newData.push_back(type);
        newData.push_back(to_string(currentScope));
        newData.push_back(newName);
        table[name] = newData;
        return noError;
    }

    // if name not in use
    vector<string> newData;
    newData.push_back(type);
    newData.push_back(to_string(currentScope));
    newData.push_back("");
    table[name] = newData;

    return noError;
}

vector<string> SymTable::getSymbol(string name) {
    // symbol dne
    if (table.find(name) == table.end()) {
        vector<string> dne;
        return dne;
    }

    return table[name];
}

bool SymTable::declaredLocally(string name) {
    // if does not exist
    if (table.find(name) == table.end()) {
        return false;
    }

    // if exists
    return true;
}