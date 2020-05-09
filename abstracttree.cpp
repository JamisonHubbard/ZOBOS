#include <iostream>
#include <string>

#include "abstracttree.h"

using std::vector;
using std::cout;
using std::string;

Node* simplifyConcreteTree(Node* root) {
    if (root->id != "PROGRAM") {
        cout << "Warning: non-PROGRAM node passed to simplifyConcreteTree() '" << root->id << "'\n";
        return root;
    }

    return simplifyNode(root);
}

vector<Node*> unwrapStatements(Node* node) {
    vector<Node*> statements;

    if (node->edges.size() == 2) {
        Node* recurse = node->edges[0];

        for (Node* child : unwrapStatements(recurse)) {
            statements.push_back(child);
        }

        statements.push_back(node->edges[1]);
    }

    delete node;
    return statements;
}

vector<Node*> unwrapDeclarations(Node* declarations) {
    vector<Node*> declIds;

    if (declarations->edges.size() == 1) {
        declIds.push_back(declarations->edges[0]);
    } else {
        Node* recurse = declarations->edges[0];

        // Delete the 'comma' node
        delete declarations->edges[1];

        for (Node* child : unwrapDeclarations(recurse)) {
            declIds.push_back(child);
        }

        declIds.push_back(declarations->edges[2]);
    }

    delete declarations;
    return declIds;
}

Node* simplifyNode(Node* node) {
    if (!node->edges.empty()) {
        for (Node*& child : node->edges) {
            child = simplifyNode(child);
        }
    }

    string id = node->id;
    if (id == "EXPR") {
        Node* child = node->edges[0];
        delete node;

        return child;
    } else if (id == "AEXPR") {
        Node* sumNode = node->edges[0];
        delete node;

        return sumNode;
    } else if (id == "BEXPR") {
        Node* left = node->edges[0];
        Node* op = node->edges[1];
        Node* right = node->edges[2];
        delete node;

        op->addEdge(simplifyNode(left));
        op->addEdge(simplifyNode(right));
        return op;
    } else if (id == "UNARY") {
        Node* op3rator = node->edges[0];
        Node* operand = node->edges[1];
        delete node;

        op3rator->addEdge(operand);
        return op3rator;
    } else if (id == "CAST") {
        // Delete the parentheses,
        // save the type and AEXPR nodes
        Node* type = node->edges[0];
        delete node->edges[1];
        Node* expr = node->edges[2];
        delete node->edges[3];
        
        delete node;

        type->addEdge(expr);
        return type;
    } else if (id == "DECLLIST") {
        Node* declType = node->edges[0];
        Node* declIds = node->edges[1];

        if (declType->id == "DECLTYPE") {
            node->edges.clear();

            node->addEdge(promoteDeclType(declType));

            for (Node* declId : unwrapDeclarations(declIds)) {
                node->addEdge(declId);
            }
        }
    } else if (id == "STATEMENT") {
        if (node->edges.size() == 2) {
            Node* sc = node->edges.back();
            node->edges.pop_back();
            delete sc;
        } else if (node->edges[0]->id == "BRACESTMTS") {
            Node* braceStatements = node->edges[0];
            delete node;
            return braceStatements;
        }
    } else if (id == "BRACESTMTS") {
        vector<Node*> statements = unwrapStatements(node->edges[1]);
        Node* open = node->edges[0];
        open->id = "open";
        Node* close = node->edges[2];
        close->id = "close";
        node->edges.clear();

        node->addEdge(open);
        for (Node* child : statements) {
            node->addEdge(child);
        }
        node->addEdge(close);
    } else if (id == "BOOLS" || id == "PLUS" || id == "TIMES") {
        node = promote(node);
    } else if (id == "EMIT") {
        node->edges.erase(node->edges.begin());
    } else if (id == "VALUE") {
        if (node->edges.size() == 3) {
            // Delete lparen and rparen
            delete node->edges[0];
            delete node->edges[2];
            Node* expr = node->edges[1];
            delete node;
            return expr;
        }

        Node* expr = node->edges[0];
        delete node;
        return expr;
    } else if (id == "ASSIGN") {
        Node* id = node->edges[0];
        Node* assign = node->edges[1];
        Node* expr = node->edges[2];
        delete node;

        assign->addEdge(id);
        assign->addEdge(expr);
        return assign;
    } else if (id == "SUM") {
        return simplifySum(node);
    } else if (id == "PRODUCT") {
        return simplifyProduct(node);
    } else if (id == "IF") {
        return simplifyIf(node);
    } else if (id == "IFELSE") {
        return simplifyIfElse(node);
    } else if (id == "WHILE") {
        return simplifyWhile(node);
    } else if (id == "PROGRAM") {
        Node* statementsNode = node->edges[0];
        Node* eof = node->edges[1];
        node->edges.clear();

        for (Node* child : unwrapStatements(statementsNode)) {
            node->addEdge(child);
        }

        node->addEdge(eof);
    } else if (id == "stringval") {
        node->val.erase(0, 1);
        node->val.erase(node->val.size() - 1);
    }

    return node;
}

Node* simplifySum(Node* sum) {
    if (sum->edges.size() == 1) {
        Node* product = sum->edges[0];
        delete sum;
        return product;
    }

    Node* left = sum->edges[0];
    Node* plus = sum->edges[1];
    Node* right = sum->edges[2];

    plus->addEdge(left);
    plus->addEdge(right);
    delete sum;
    return plus;
}

Node* simplifyProduct(Node* product) {
    if (product->edges.size() == 1) {
        Node* val = product->edges[0];
        delete product;
        return val;
    }

    Node* left = product->edges[0];
    Node* times = product->edges[1];
    Node* right = product->edges[2];

    times->addEdge(left);
    times->addEdge(right);
    delete product;
    return times;
}

Node* simplifyIf(Node* ifNode) {
    // Delete the 'if' word and the parentheses
    // and save the BEXPR and STATEMENT
    delete ifNode->edges[0];
    Node* bexpr = ifNode->edges[2];
    delete ifNode->edges[1];
    delete ifNode->edges[3];
    Node* stmt = ifNode->edges[4];

    ifNode->edges.clear();

    ifNode->addEdge(bexpr);
    ifNode->addEdge(stmt);
    return ifNode;
}

Node* simplifyIfElse(Node* ifElseNode) {
    // Delete the 'if', 'else' and parentheses
    // Save the BEXPR and statements
    delete ifElseNode->edges[0];
    delete ifElseNode->edges[1];
    Node* bexpr = ifElseNode->edges[2];
    delete ifElseNode->edges[3];
    Node* ifStatements = ifElseNode->edges[4];
    delete ifElseNode->edges[5];
    Node* elseStatements = ifElseNode->edges[6];

    ifElseNode->edges.clear();
    ifElseNode->addEdge(bexpr);
    ifElseNode->addEdge(ifStatements);
    ifElseNode->addEdge(elseStatements);

    return ifElseNode;
}

Node* simplifyWhile(Node* whileNode) {
    // Delete the 'while' and parentheses
    // Save the BEXPR and statement(s)
    delete whileNode->edges[0];
    delete whileNode->edges[1];
    Node* bexpr = whileNode->edges[2];
    delete whileNode->edges[3];
    Node* statements = whileNode->edges[4];
    whileNode->edges.clear();

    whileNode->addEdge(bexpr);
    whileNode->addEdge(statements);
    return whileNode;
}

Node* promoteDeclType(Node* declType) {
    if (declType->edges.size() == 1) {
        Node* type = declType->edges[0];
        delete declType;

        return type;
    } else {
        Node* type = declType->edges[0];

        for (uint i = 1; i < declType->edges.size(); i++) {
            type->id += " " + declType->edges[i]->id;
            type->val += " " + declType->edges[i]->val;
            delete declType->edges[i];
        }
        delete declType;

        return type;
    }
}

Node* promote(Node* node) {
    if (node->edges.size() != 1) {
        return node;
    }

    Node* child = node->edges[0];
    delete node;
    return child;
}