#include <string>
#include <vector>
#include <iostream>

#include "node.h"

using std::cout;

const Location Location::NONE = { -1, -1 };

void Node::clearEdges() {
    for (Node* node : edges) {
        node->clearEdges();
    }

    while(edges.size() > 0) {
        Node* node = edges[edges.size()-1];
        edges.pop_back();
        delete node;
    }
}

void Node::printEdges(int numTabs) {
    for (int i = 0; i < numTabs; ++i) std::cout << "\t";

    cout << "ID: " << id << " - ";
    cout << "VAL: " << val << std::endl;

    for (int i = 0; i < edges.size(); ++i) {
        edges[i]->printEdges(numTabs+1);
    }
}