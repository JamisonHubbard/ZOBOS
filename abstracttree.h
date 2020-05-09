#ifndef ABSTRACT_TREE_H
#define ABSTRACT_TREE_H

#include <functional>
#include <vector>

#include "grammar.h"
#include "node.h"

Node* simplifyConcreteTree(Node*);
std::vector<Node*> unwrapStatements(Node*);
std::vector<Node*> unwrapDeclarations(Node*);

Node* simplifyNode(Node*);

Node* simplifyIf(Node*);
Node* simplifyIfElse(Node*);
Node* simplifyWhile(Node*);

Node* simplifySum(Node*);
Node* simplifyProduct(Node*);

Node* promoteDeclType(Node*);

Node* promote(Node*);

#endif // ABSTRACT_TREE_H