/*
    Jamison Hubbard
    March 2020
    CFG Implementation
    grammar.h
*/

#ifndef GRAMMAR_H
#define GRAMMAR_H

using namespace std;

#include <string>
#include <map>
#include <set>
#include "rule.h"
#include "node.h"
#include "srAction.h"
#include "error.h"

typedef unsigned int uint;

class Grammar {
public:
    // Constructor
    Grammar(string filename);

    // Access Functions
    set<string> getNonTerminals();
    set<string> getTerminals();
    Rule getRule(int ruleID);
    int getRuleID(Rule rule);
    int numRules() const;
    string getStartSymbol() const;
    map<string, vector<int>> getLL1Table() const;
    map<string, int> getLL1Indices() const;
    bool isAmbiguous() const;
    
    // Algorithms
    set<string> firstSet(string nonterminal);
    set<string> followSet(string symbol);
    bool derivesToLambda(string nonterminal);
    set<string> predictSet(int ruleID);
    void generateLL1Table();

    // ZOBOS
    Node* parseString(map<int, map<string, srAction>> slrTable, string tokenStreamFile);
    vector<Error> semanticChecks(Node* head, string fileDump);
    vector<Error> recursiveCheck(Node* current, SymTable &symtable, string fileDump);

    vector<Error> braceStmt(Node* current, SymTable &symtable);
    vector<Error> declStmt(Node* current, SymTable &symtable);
    vector<Error> declIdStmt(Node* current, SymTable &symtable, string type);
    vector<Error> assignStmt(Node* current, SymTable &symtable);
    vector<Error> assignStmt(Node* current, SymTable &symtable, string type);
    vector<Error> exprStmt(Node* current, SymTable &symtable, string &type);
    vector<Error> ifStmt(Node* current, SymTable &symtable);
    vector<Error> ifElseStmt(Node* current, SymTable &symtable);
    vector<Error> whileStmt(Node* current, SymTable &symtable);
    vector<Error> emitStmt(Node* current, SymTable &symtable, string fileDump);

    string isOperator(string op);
    void dumpSymTable(string fileDump, SymTable symtable);

    // Other
    void print();
    void report();
    void printLL1Table() const;
    string trimEdges(string in);
    set<Rule> getRulesDerivedFrom(string nonterminal);
    set<Rule> getRulesDerivedTo(string symbol);
    bool isTerminal(string symbol);
    bool isNonTerminal(string symbol);
    bool rulesEqual(Rule rOne, Rule rTwo);

private:
    map<int, Rule> rules;
    int ruleCount;

    set<string> nonterminals;
    set<string> terminals;
    string startSymbol;

    map<string, vector<int>> ll1_table;
    map<string, int> ll1_index;
    bool ambiguous;
};

#endif /*GRAMMAR_H*/