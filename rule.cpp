/*
    Jamison Hubbard
    March 2020
    CFG Implementation
    rule.cpp
*/

using namespace std;

#include <string>
#include "rule.h"

// Constructors
Rule::Rule() {
    first = "";
    symbolCount = 0;
}
Rule::Rule(string lhs, vector<string> rhs) {
    first = lhs;
    second = rhs;
    symbolCount = second.size();
}

// Access Functions
string Rule::getLHS() {return first;}
vector<string> Rule::getRHS() {return second;}
int Rule::getSymbolCount() {return symbolCount;}

// Other
bool Rule::operator<(const Rule &other) const {
    // this operator is defined because std::set containers use the
    // < operator to determine if two objects are the same (I think).
    // So this operator tests if the rules are the same and returns
    // true if they're not so std::set handles them as different.

    if (symbolCount != other.symbolCount) return true;
    if (first != other.first) return true;
    for (int i = 0; i < second.size(); ++i) {
        if (second[i] != other.second[i]) return true;
    }
    return false;
}