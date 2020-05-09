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

string Rule::asString() const {
    string thisStr = "";
    thisStr += first + " -> ";
    for (string symbol : second) {
        thisStr += symbol + " ";
    }

    return thisStr;
}

// Other
bool Rule::operator<(const Rule &other) const {
    // this operator is defined because std::set containers use the
    // < operator to determine if two objects are the same (I think).
    // So this operator tests if the rules are the same and returns
    // true if they're not so std::set handles them as different.

    string thisStr = this->asString();
    string otherStr = other.asString();

    return thisStr < otherStr;
}

std::ostream& operator<<(std::ostream& out, const Rule rule) {
    return out << rule.asString();
}