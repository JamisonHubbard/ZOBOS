#include <string>
#include <vector>

#ifndef NODE_H
#define NODE_H

class Node {
public:
    Node(std::string name, std::string value) {id=name; val=value;}
    std::string getID() {return id;}
    std::string getVal() {return val;}
    std::vector<Node*> getEdges() {return edges;}

    void addEdge(Node* newEdge) {edges.push_back(newEdge);}
    void clearEdges();
    void printEdges(int numTabs);
private:
    std::string id;
    std::string val;
    std::vector<Node*> edges;
};

#endif /*NODE_H*/