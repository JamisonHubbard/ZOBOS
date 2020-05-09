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

class Rule {
public:
    // Constructors
    Rule();
    Rule(string lhs, vector<string> rhs);

    // Access Functions
    string getLHS();
    vector<string> getRHS();
    int getSymbolCount();

    // Other
    bool operator<(const Rule &other) const;

private:
    string first;
    vector<string> second;
    int symbolCount;
};

#endif /*RULE_H*/