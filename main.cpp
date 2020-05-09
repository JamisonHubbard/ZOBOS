/*
    Jamison Hubbard
    March 2020
    CFG Implementation
    rule.cpp
*/

using namespace std;

#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include "srAction.h"
#include "grammar.h"
#include "abstracttree.h"

void writeAST(Node* node, const string filename);
void traverseAST(Node* node, string address, string& first, string& second);
map<int, map<string, srAction>> readSLRTable(string filename);

int main(int argc, char *argv[]) {
    if (argc != 4 && argc != 6) exit(1);

    string tokenStreamFile = argv[1];
    string astFile = argv[2];
    string symTableFile = argv[3];
    string grammarFile, slrFile;
    if (argc == 6) {
        grammarFile = argv[4];
        slrFile = argv[5];
    } else {
        grammarFile = "./provided/zlang-pure.cfg";
        slrFile = "./provided/zlang.lr";
    }

    Grammar g(grammarFile);
    map<int, map<string, srAction>> slrTable = readSLRTable(slrFile);

    Node* head = g.parseString(slrTable, tokenStreamFile);

    Node* ast = simplifyConcreteTree(head);
    writeAST(ast, astFile);

    head->printEdges(0);
    
    head->clearEdges();
    delete head;

    return 0;
}

void writeAST(Node* root, const string filename) {
    string firstPart, secondPart;

    traverseAST(root, "0", firstPart, secondPart);

    ofstream outFile(filename);
    if (!outFile) {
        exit(1);
    }

    outFile << firstPart;
    outFile << "\n";
    outFile << secondPart;
    outFile.close();
}

void traverseAST(Node* node, string address, string& first, string& second) {
    if (node->val == "NONTERMINAL") {
        first += address + " " + node->id + "\n";
    } else {
        first += address + " " + node->val + "\n";
    }

    if (!node->edges.empty()) {
        for (uint i = 0; i < node->edges.size(); i++) {
            Node* child = node->edges[i];
            string childAddress = address + "-" + to_string(i);

            second += address + " " + childAddress + "\n";
            traverseAST(child, childAddress, first, second);
        }
    }
}

map<int, map<string, srAction>> readSLRTable(string filename) {
    map<int, map<string, srAction>> slrTable;
    
    ifstream inFile(filename);
    if (!inFile) exit(1);

    string first;
    getline(inFile, first);
    stringstream inFirst(first + ",");

    vector<string> symbolOrder;
    string currentSym = "";
    char holder;

    while (inFirst >> holder) {
        if (holder == '.') continue;
        if (holder == ',') {
            symbolOrder.push_back(currentSym);
            currentSym = "";
            continue;
        }
        
        currentSym += holder;
    }

    string line;
    while (getline(inFile, line)) {
        map<string, srAction> tableLine;
        string indexString;
        stringstream inLine(line + ",");

        while (inLine >> holder) {
            if (holder == ',') break;
            indexString += holder;
        }
        int stateIndex = stoi(indexString);
        currentSym = "";
        int symbolIndex = 1;
        while (inLine >> holder) {
            if (holder == ',' || holder == '\n') {
                srAction newAction;
                if (currentSym == "") {
                    newAction.status = false;
                    newAction.action = "";
                    newAction.ruleID = -1;
                }
                else if (currentSym[0] == 'r' || currentSym[0] == 'R') {
                    newAction.status = true;
                    newAction.action = "reduce";
                    newAction.ruleID = stoi(currentSym.substr(2, currentSym.length()-2));
                }
                else if (currentSym[0] == 's') {
                    newAction.status = true;
                    newAction.action = "shift";
                    newAction.ruleID = stoi(currentSym.substr(3, currentSym.length()-3));
                }
                tableLine.insert(pair<string, srAction>(symbolOrder[symbolIndex], newAction));
                ++symbolIndex;
                currentSym = "";
                continue;
            }
            
            currentSym += holder;
        }

        slrTable.insert(pair<int, map<string, srAction>>(stateIndex, tableLine));
    }

    inFile.close();
    return slrTable;
}