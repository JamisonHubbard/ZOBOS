/*
    Jamison Hubbard
    March 2020
    CFG Implementation
    rule.h
*/

#ifndef RULE_H
#define RULE_H

using namespace std;

#include <string>
#include <vector>
#include <ostream>

class Rule {
public:
    // Constructors
    Rule();
    Rule(string lhs, vector<string> rhs);

    // Access Functions
    string getLHS();
    vector<string> getRHS();
    int getSymbolCount();
    string asString() const;

    // Other
    bool operator<(const Rule &other) const;

    friend std::ostream& operator<<(std::ostream& out, const Rule rule);
private:
    string first;
    vector<string> second;
    int symbolCount;
};

#endif /*RULE_H*/