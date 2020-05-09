/*
    Jamison Hubbard
    March 2020
    CFG Implementation
    grammar.cpp
*/

using namespace std;

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>
#include "rule.h"
#include "grammar.h"
#include "error.h"
#include "symTable.h"

// Constructor
Grammar::Grammar(string filename) {
    // open file stream to given CFG file
    ifstream fileIn(filename);
    if (!fileIn) exit(1);

    // this bool will allow us the save the start symbol
    bool isStart = true;

    // setup a while loop to read line-by-line, with rule counter
    string line;
    string currentNT;
    int ruleCounter = 0;
    while(getline(fileIn, line)) {
        // read the first two words to see what line format
        // the current line is
        stringstream lineIn(line);
        string first, second;
        lineIn >> first;
        lineIn >> second;

        // if an empty line move on
        if (first == "" && second == "") continue;

        if (second == "->") {
            // set new nonterminal and get rhs of rule
            currentNT = trimEdges(first);
            nonterminals.insert(currentNT);
            vector<string> rhs;
            string word = " ";

            // if new NT is the start symbol, save it
            if (isStart) {
                isStart = false;
                startSymbol = currentNT;
            }

            // while the line still has unread words
            while (word != "") {
                word = "";
                lineIn >> word;
                if (word != "|") {
                    if (word != "") {
                        rhs.push_back(word);
                        terminals.insert(word);
                    }
                } else {
                    // if a "|" is found create the new rule
                    // and reset to start a new one

                    Rule newRule(currentNT, rhs);
                    rules.insert(pair<int, Rule>(ruleCounter, newRule));
                    ++ruleCounter;
                    while (rhs.size() > 0) {rhs.pop_back();}
                }
            }

            if (rhs.size() > 0) {
                // create new rule and add it to the rule set
                Rule newRule(currentNT, rhs);
                rules.insert(pair<int, Rule>(ruleCounter, newRule));
                ++ruleCounter;
            }
        } else if (first == "|") {
            // use second to start the rhs
            vector<string> rhs;
            rhs.push_back(second);
            terminals.insert(second);
            string word = " ";

            // while the line still has unread words
            while (word != "") {
                word = "";
                lineIn >> word;
                if (word != "|") {
                    if (word != "") {
                        rhs.push_back(word);
                        terminals.insert(word);
                    }
                } else {
                    // if a "|" is found create the new rule
                    // and reset to start a new one

                    Rule newRule(currentNT, rhs);
                    rules.insert(pair<int, Rule>(ruleCounter, newRule));
                    ++ruleCounter;
                    while (rhs.size() > 0) {rhs.pop_back();}
                }
            }

            if (rhs.size() > 0) {
                // create new rule and add it to the rule set
                Rule newRule(currentNT, rhs);
                rules.insert(pair<int, Rule>(ruleCounter, newRule));
                ++ruleCounter;
            }
        } else {
            cout << "\n\nIncorrectly formatted file at rule ";
            cout << ruleCount << "\n\n";
            exit(2);
        }
    }

    // correct any nonterminals that were placed in the
    // terminals set, as well as $ and lambda
    set<string> removeFromTerminalSet;
    for (string nt : nonterminals) {
        for (string t : terminals) {
            if (nt == t) {
                removeFromTerminalSet.insert(nt);
                break;
            }
        }
    }
    for (string t : terminals) {
        if (t == "$" || t == "lambda") {
            removeFromTerminalSet.insert(t);
        }
    }
    for (string nt : removeFromTerminalSet) {
        terminals.erase(nt);
    }

    fileIn.close();
    ruleCount = ruleCounter;

    // generate the LL(1) Table
    // this will also generate the LL1 Index, and determine the
    // grammar's ambiguity state

    generateLL1Table();

}

// Access Functions
set<string> Grammar::getNonTerminals() {
    return nonterminals;
}

set<string> Grammar::getTerminals() {
    return terminals;
}

Rule Grammar::getRule(int ruleID) {
    if (ruleID >= ruleCount) {
        return Rule();
    }
    
    return rules[ruleID];
}

int Grammar::getRuleID(Rule rule) {
    for (pair<int, Rule> production : rules) {
        if (rulesEqual(rule, production.second)) return production.first; 
    }
    return -1;
}

int Grammar::numRules() const {
    return ruleCount;
}

string Grammar::getStartSymbol() const {
    return startSymbol;
}

map<string, vector<int>> Grammar::getLL1Table() const {
    return ll1_table;
}

map<string, int> Grammar::getLL1Indices() const {
    return ll1_index;
}

bool Grammar::isAmbiguous() const {
    return ambiguous;
}

// Algorithms
set<string> Grammar::firstSet(string nonterminal) {
    // if not a nonterminal, return itself
    // unless it's lambda, then return an empty set
    if (!isNonTerminal(nonterminal)) {
        set<string> notNonTerminal;
        if (nonterminal != "lambda") notNonTerminal.insert(nonterminal);
        return notNonTerminal;
    }

    // create set to contain terminals in firstSet
    set<string> first;

    // iterate through rules with nonterminal on LHS
    set<Rule> rulesFromNT = getRulesDerivedFrom(nonterminal);
    for (Rule r : rulesFromNT) {
        // find firstSet(NT) and add it to set
        // if NT derivesToLambda, then bump forward and
        // repeat entire process
        uint front = 0;

        while(true) {
            // look at current symbol on RHS
            string currentSymbol = r.getRHS()[front];

            // if a T, add it to set and move on to next rule
            if (isTerminal(currentSymbol)) {
                first.insert(currentSymbol);
                break;
            }

            // if it's lambda or $, also move on to next rule
            if (currentSymbol == "lambda") break;

            // check to make sure it's an NT then
            if (!isNonTerminal(currentSymbol)) {
                exit(4);
            }

            // if current NT, move on to next rule
            if (currentSymbol == nonterminal) {
                break;
            }

            // otherwise, find first(NT) and add it to set
            set<string> firstOfCurrent = firstSet(currentSymbol);
            for (string symbol : firstOfCurrent) {
                first.insert(symbol);
            }

            // check if NT can derive to lambda
            // if so, adjust front and repeat, otherwise break
            if (derivesToLambda(currentSymbol)) {
                ++front;
                
                // if front exceeds the number of symbols
                // on the RHS, then we need to break
                if (front >= r.getRHS().size()) break;
                continue;
            } else {
                break;
            }

        }
    }

    // return the finished set
    return first;
}

set<string> Grammar::followSet(string symbol) {
    // return an empty set if lambda or $
    if (symbol == "lambda" || symbol == "$") {
        set<string> emptySet;
        return emptySet;
    }

    // create set to contain terminals in the follow set
    set<string> follow;

    // iterate through rules with symbol on the RHS
    set<Rule> rulesToSymbol = getRulesDerivedTo(symbol);
    for (Rule r : rulesToSymbol) {
        // find the "index" of the symbol in the rhs of the rule
        uint symbolIndex = 1;
        for (uint i = 0; i < r.getRHS().size(); ++i) {
            if (r.getRHS()[i] == symbol) {
                symbolIndex = i;
                break;
            }
        }

        // if the symbol was the last element, we need to find the
        // follow set of the nonterminal for the current rule
        vector<string> rhs = r.getRHS();
        bool allDeriveToLambda = all_of(
            rhs.begin() + symbolIndex + 1,
            rhs.end(),
            [&](string x) { return derivesToLambda(x); });
        if (rhs.size() == symbolIndex + 1 || allDeriveToLambda) {
            // unless the nonterminal is also the symbol
            if (r.getLHS() == symbol) continue;

            set<string> followOfNT = followSet(r.getLHS());
            for (string terminal : followOfNT) {
                follow.insert(terminal);
            }
        }

        if (symbolIndex + 1 < rhs.size()) {
            // get the next symbol
            string nextSymbol = r.getRHS()[symbolIndex + 1];

            // check if the next symbol is a terminal or $. If so,
            // add it to set and move on
            if (isTerminal(nextSymbol) || nextSymbol == "$") {
                follow.insert(nextSymbol);
                continue;
            }

            // if it's lambda move on
            if (nextSymbol == "lambda") { continue; }

            // now it has to be a nonterminal, so find its
            // first set and add that
            set<string> firstOfNext = firstSet(nextSymbol);
            for (string terminal : firstOfNext) {
                follow.insert(terminal);
            }
        }
    }

    // return the follow set
    return follow;
}

bool Grammar::derivesToLambda(const string nonterminal) {
    // if not a nonterminal, return false
    // unless it's lambda, then return true
    if (!isNonTerminal(nonterminal)) {
        if (nonterminal == "lambda") return true;
        return false;
    }

    // iterate through rules with nonterminal on LHS
    set<Rule> rulesFromNT = getRulesDerivedFrom(nonterminal);
    for (Rule r : rulesFromNT) {
        // if rule is simply lambda return true
        if (r.getRHS().size() == 1 && r.getRHS()[0] == "lambda") return true;

        // if any symbol is a terminal or cannot
        // deriveToLambda, then move on
        bool cannotD2L = false;
        for (uint i = 0; i < r.getRHS().size(); ++i) {
            // check if symbol on lhs is the rhs nonterminal
            if (r.getRHS()[i] == nonterminal) continue;
            if (!derivesToLambda(r.getRHS()[i])) {
                cannotD2L = true;
                break;
            }
        }

        if (cannotD2L) continue;
        else return true;
    }

    // if all rules have been seen and non deriveToLambda
    return false;
}
set<string> Grammar::predictSet(int ruleID) {
    Rule rule = rules[ruleID];

    // create a set to hold the terminals in the predict set
    set<string> predict;

    // find the first set a symbol in the RHS, starting at the first
    // one, and add it to the predict set. If the symbol doesn't
    // derive to lambda, break, otherwise continue.

    bool allDeriveToLambda = true;

    for (uint i = 0; i < rule.getRHS().size(); ++i) {
        set<string> firstOfSymbol = firstSet(rule.getRHS()[i]);

        
        for (string terminal : firstOfSymbol) {
            predict.insert(terminal);
        }
        if (!derivesToLambda(rule.getRHS()[i])) {
            allDeriveToLambda = false;
            break;
        }
    }

    // if not all derive to lambda, then return the predict set
    if (!allDeriveToLambda) return predict;

    // otherwise, find the follow set of the nonterminal and add
    // that as well to the predict set

    set<string> followOfNT = followSet(rule.getLHS());
    for (string terminal : followOfNT) {
        predict.insert(terminal);
    }

    // return the predict set
    return predict;
}

void Grammar::generateLL1Table() {
    // at the beginning, set ambiguous to false
    ambiguous = false;

    // fill the table with -1 for the correct sizing
    // the table has rows for each nonterminal, and columns for
    // each terminal + $
    for (string nt : nonterminals) {
        vector<int> columns;
        for (uint i = 0; i < terminals.size() + 1; ++i) {
            columns.push_back(-1);
        }
        ll1_table.insert(pair<string, vector<int>>(nt, columns));
    }

    // map each terminal to an index for the columns
    // $ will always be last
    int indexCount = 0;
    for (string t : terminals) {
        ll1_index.insert(pair<string, int>(t, indexCount));
        ++indexCount;
    }
    ll1_index.insert(pair<string, int>("$", indexCount));

    
    // for each rule, get its predict set
    for (pair<int, Rule> rule : rules) {
        set<string> predict = predictSet(rule.first);
        
        // get the nonterminal for the rule
        string nt = rule.second.getLHS();

        // for each symbol in the predict set, place the rule number
        // in the table in the row corresponding to the nt
        // and the column given by the index map
        for (string symbol : predict) {
            // if the cell already has a value, the grammar is ambiguous
            if (ll1_table[nt][ll1_index[symbol]] != -1) ambiguous = true;

            ll1_table[nt][ll1_index[symbol]] = rule.first;
        }
    }
}

// Other
void Grammar::print() {
    for (int i = 0; i < ruleCount; ++i) {
        cout << to_string(i) << "\t";
        cout << rules[i].getLHS() << " ->";
        vector<string> rhs = rules[i].getRHS();
        for (uint j = 0; j < rhs.size(); ++j) {
            cout << " " << rhs[j];
        }
        cout << endl;
    }
}
void Grammar::report() {
    // print cfg
    print();

    // print nonterminals
    cout << "\nNonterminals: ";
    for (string nt : nonterminals) {
        cout << nt << " ";
    }

    // print terminals
    cout << "\nTerminals: ";
    for (string t : terminals) {
        cout << t << " ";
    }

    // print all nonterminals that derive to lambda
    cout << "\n\nDerives To Lambda: ";
    for (string nt : nonterminals) {
        if (derivesToLambda(nt)) cout << nt << " ";
    }

    // print all first sets of nonterminals
    cout << endl << endl;
    for (string nt : nonterminals) {
        cout << "First(" << nt << "):";
        set<string> first = firstSet(nt);

        for (string symbol : first) {
            cout << " " << symbol;
        }
        cout << endl;
    }

    // print all follow sets of nonterminals
    cout << endl << endl;
    for (string nt : nonterminals) {
        cout << "Follow(" << nt << "):";
        set<string> follow = followSet(nt);

        for (string symbol : follow) {
            cout << " " << symbol;
        }
        cout << endl;
    }

    // print all predict sets of rules
    cout << endl << endl;
    for (pair<int, Rule> rule : rules) {
        cout << "Predict(" << rule.second.getLHS() << "):";

        set<string> predict = predictSet(rule.first);
        for (string terminal : predict) {
            cout << " " << terminal;
        }
        cout << endl;
    }

    // print ambiguity status
    cout << "\nAmbiguous: ";
    if (ambiguous) cout << "YES\n";
    else cout << "NO\n";

    // if not ambiguous, print the LL(1) table
    if (!ambiguous) {
        printLL1Table();
    }
    
    cout << endl;
}

void Grammar::printLL1Table() const {
    // set a spacing string
    string spacing = "\t";

    // output all the terminals
    cout << endl << "LL(1) Parse Table" << endl << endl << spacing;
    for (string t : terminals) {
        cout << t << spacing;
    }
    cout << "$" << endl << endl;

    // for each row in the table, output the nt and all the numbers
    for (pair<string, vector<int>> row : ll1_table) {
        cout << row.first;
        for (uint i = 0; i< row.second.size(); ++i) {
            cout << spacing;
            if (row.second[i] == -1) cout << " ";
            else cout << row.second[i];
        }
        cout << endl << endl;
    }
}

string Grammar::trimEdges(string in) {
    // trim excess spaces from front and back of string
    while (in[0] == ' ') {
        in = in.substr(1, in.length()-1);
    }

    while (in[in.length()-1] == ' ') {
        in = in.substr(0, in.length()-1);
    }

    return in;
}

set<Rule> Grammar::getRulesDerivedFrom(string nonterminal) {
    // create set to return
    set<Rule> rulesFromNT;
    
    // iterate through rules adding ones with nonterminal
    // as the LHS to the set
    for (pair<int, Rule> rule : rules) {
        string lhs = rule.second.getLHS();
        if (lhs == nonterminal) rulesFromNT.insert(rule.second);
    }

    // return set
    return rulesFromNT;
}

set<Rule> Grammar::getRulesDerivedTo(string symbol) {
    // create set to return
    set<Rule> rulesToSymbol;
    
    // iterate through rules adding ones with nonterminal
    // as the LHS to the set
    for (pair<int, Rule> rule : rules) {
        vector<string> rhs = rule.second.getRHS();
        for (uint i = 0; i < rhs.size(); ++i) {
            if (rhs[i] == symbol) {
                rulesToSymbol.insert(rule.second);
                break;
            }
        }
    }

    // return set
    return rulesToSymbol;
}

bool Grammar::isTerminal(string symbol) {
    if (symbol == "$") {
        return true;
    }

    for (string t : terminals) {
        if (symbol == t) return true;
    }

    return false;
}

bool Grammar::isNonTerminal(string symbol) {
    for (string nt : nonterminals) {
        if (symbol == nt) return true;
    }

    return false;
}

bool Grammar::rulesEqual(Rule rOne, Rule rTwo) {
    if (rOne.getLHS() != rTwo.getLHS()) return false;
    if (rOne.getSymbolCount() != rTwo.getSymbolCount()) return false;

    for (int i = 0; i < rOne.getSymbolCount(); ++i) {
        if (rOne.getRHS()[i] != rTwo.getRHS()[i]) return false;
    }

    return true;
}

Node* Grammar::parseString(map<int, map<string, srAction>> slrTable, string tokenStreamFile) {
    fstream inFile(tokenStreamFile);
    if (!inFile) exit(1);

    vector<Node*> left;
    vector<Node*> right;

    // put 10 $ symbols on right stack as buffer
    for (int i = 0; i < 10; ++i) {
        Node* dollarNode = new Node("$", "$");
        right.push_back(dollarNode);
    }

    Node* startState = new Node("STATE", "0");
    left.push_back(startState);

    string line;
    streampos prevLine = inFile.tellg();
    map<Node*, vector<int>> srcLocs;
    while (getline(inFile, line)) {
        
        // get node from left stack
        Node* leftTip = left[left.size()-1];

        // prepare to pull from token stream if needed
        istringstream inLine(line);
        string tokenID, tokenVal;
        inLine >> tokenID;
        inLine >> tokenVal;

        // get node from right stack, or toekn stream
        // it the right stack is empty
        Node* rightTip;
        if (right.size() == 10) {
            rightTip = new Node(tokenID, tokenVal);
            right.push_back(rightTip);
            prevLine = inFile.tellg();
            int row, col;
            inLine >> row;
            inLine >> col;
            vector<int> location;
            location.push_back(row);
            location.push_back(col);
            srcLocs.insert(pair<Node*, vector<int>>(rightTip, location));
        } 
        else {
            rightTip = right[right.size() - 1];
            inFile.seekg(prevLine);
        }

        // read SLR table
        int tableRow = stoi(leftTip->getVal());
        string tableCol = rightTip->getID();
        srAction action = slrTable[tableRow][tableCol];

        // syntax error
        if (!(action.status)) {
            vector<int> location = srcLocs[rightTip];
            string locData = to_string(location[0]) + " " + to_string(location[1]);
            Node* error = new Node("SYNTAX", locData);
            return error;
        }
        // shift
        if (action.action == "shift") {
            string newStateID = to_string(action.ruleID);
            Node* newState = new Node("STATE", newStateID);
            newState->addEdge(rightTip);
            left.push_back(newState);
            right.pop_back();
        }
        // reduce
        if (action.action == "reduce") {
            Rule reduceRule = rules[action.ruleID - 1];

            // if a lambda rule
            if (reduceRule.getRHS()[0] == "lambda") {
                Node* newNT = new Node(reduceRule.getLHS(), "NONTERMINAL");
                Node* lambdaNode = new Node("lambda", "lambda");
                newNT->addEdge(lambdaNode);
                right.push_back(newNT);
                continue;
            }
            
            vector<string> leftTopSymbols;
            int displacement = 0;
            while (leftTopSymbols.size() < reduceRule.getRHS().size()) {
                ++displacement;
                
                Node* someLeftState = left[left.size() - displacement];
                vector<Node*> edges = someLeftState->getEdges();

                while (edges.size() > 0) {
                    Node* lastEdge = edges[edges.size()-1];
                    edges.pop_back();
                    leftTopSymbols.push_back(lastEdge->getID());
                }
            }
            if (reduceRule.getRHS().size() != leftTopSymbols.size()) {
                // syntax error, reduction can't be done
                vector<int> location = srcLocs[rightTip];
                string locData = to_string(location[0]) + " " + to_string(location[1]);
                Node* error = new Node("SYNTAX", locData);
                return error;
            }

            // collect the left states that need reduced
            vector<Node*> statesToReduce;
            for (int i = 0; i < displacement; ++i) {
                statesToReduce.push_back(left[left.size()-1]);
                left.pop_back();
            }

            // now strip off the state nodes that head the trees
            vector<Node*> nodesToReduce;
            for (int i = statesToReduce.size()-1; i > -1; --i) {
                vector<Node*> nodes = statesToReduce[i]->getEdges();
                for (int i = 0; i < nodes.size(); ++i) {
                    nodesToReduce.push_back(nodes[i]);
                }

                delete statesToReduce[i];
                statesToReduce.pop_back();
            }

            // create new nonterminal node as parent
            Node* parent = new Node(reduceRule.getLHS(), "NONTERMINAL");
            for (int i = 0; i < nodesToReduce.size(); ++i) {
                parent->addEdge(nodesToReduce[i]);
            }

            // add new tree to right stack
            right.push_back(parent);

            // add location data of children for new
            // parent node
            vector<int> childLoc = srcLocs[nodesToReduce[0]];
            srcLocs[parent] = childLoc;
        }
    }

    // when the loop exits, the token stream is empty
    // but the process is not
    while (right[right.size()-1]->getID() != startSymbol) {

        // get nodes from respective stacks
        Node* leftTip = left[left.size()-1];
        Node* rightTip = right[right.size() - 1];

        // read SLR table
        int tableRow = stoi(leftTip->getVal());
        string tableCol = rightTip->getID();
        srAction action = slrTable[tableRow][tableCol];

        // syntax error
        if (!(action.status)) {
            vector<int> location = srcLocs[rightTip];
            string locData = to_string(location[0]) + " " + to_string(location[1]);
            Node* error = new Node("SYNTAX", locData);
            return error;
        }
        // shift
        if (action.action == "shift") {
            string newStateID = to_string(action.ruleID);
            Node* newState = new Node("STATE", newStateID);
            newState->addEdge(rightTip);
            left.push_back(newState);
            right.pop_back();
        }
        // reduce
        if (action.action == "reduce") {
            Rule reduceRule = rules[action.ruleID - 1];

            // if a lambda rule
            if (reduceRule.getRHS()[0] == "lambda") {
                Node* newNT = new Node(reduceRule.getLHS(), "NONTERMINAL");
                Node* lambdaNode = new Node("lambda", "lambda");
                newNT->addEdge(lambdaNode);
                right.push_back(newNT);
                continue;
            }
            
            vector<string> leftTopSymbols;
            int displacement = 0;
            while (leftTopSymbols.size() < reduceRule.getRHS().size()) {
                ++displacement;
                
                Node* someLeftState = left[left.size() - displacement];
                vector<Node*> edges = someLeftState->getEdges();

                while (edges.size() > 0) {
                    Node* lastEdge = edges[edges.size()-1];
                    edges.pop_back();
                    leftTopSymbols.push_back(lastEdge->getID());
                }
            }
            if (reduceRule.getRHS().size() != leftTopSymbols.size()) {
                // syntax error, reduction can't be done
                vector<int> location = srcLocs[rightTip];
                string locData = to_string(location[0]) + " " + to_string(location[1]);
                Node* error = new Node("SYNTAX", locData);
                return error;
            }

            // collect the left states that need reduced
            vector<Node*> statesToReduce;
            for (int i = 0; i < displacement; ++i) {
                statesToReduce.push_back(left[left.size()-1]);
                left.pop_back();
            }

            // now strip off the state nodes that head the trees
            vector<Node*> nodesToReduce;
            for (int i = statesToReduce.size()-1; i > -1; --i) {
                vector<Node*> nodes = statesToReduce[i]->getEdges();
                for (int i = 0; i < nodes.size(); ++i) {
                    nodesToReduce.push_back(nodes[i]);
                }

                delete statesToReduce[i];
                statesToReduce.pop_back();
            }

            // create new nonterminal node as parent
            Node* parent = new Node(reduceRule.getLHS(), "NONTERMINAL");
            for (int i = 0; i < nodesToReduce.size(); ++i) {
                parent->addEdge(nodesToReduce[i]);
            }

            // add new tree to right stack
            right.push_back(parent);
        }
    }

    // grab the head of the final tree
    Node* finalNode = right[right.size()-1];
    right.pop_back();

    // cleanup
    for (int i = 0; i < left.size(); ++i) {
        left[i]->clearEdges();
        delete left[i];
    }
    for (int i = 0; i < right.size(); ++i) {
        right[i]->clearEdges();
        delete right[i];
    }

    return finalNode;
}

vector<Error> Grammar::semanticChecks(Node* head) {
    SymTable symtable;

    vector<Error> totalErrors = recursiveCheck(head, symtable);

    return totalErrors;
}

vector<Error> Grammar::recursiveCheck(Node* current, SymTable &symtable) {
    vector<Error> myErrors;

    // if in a declaration
    if (current->getID() == "DECLLIST") {
        // two children
        Node* typeChild = current->getEdges()[0];
        Node* namesChild = current->getEdges()[1];

        // get type
        string type = typeChild->getID();

        vector<string> names;
        // get name from one reference
        if (namesChild->getID() == "DECLID") {
            Node* namesGrandChild = namesChild->getEdges[0];
            // no assignment
            if (namesGrandChild->getID() == "id") {
                names.push_back(namesGrandChild->getVal());
            }
            // with assignment
            
        }
    }

    // recursively descend through children
    for (int i = 0; i < current->getEdges().size(); ++i) {
        Node* child = current->getEdges[i];
        vector<Error> newErrors = recursiveCheck(child, symtable, state);
        for (Error newE : newErrors) myErrors.push_back(newE);
    }

    return myErrors;
}