#include <string>
#include <vector>

#ifndef NODE_H
#define NODE_H

struct Location {
    const static Location NONE;

    int row;
    int col;

    bool operator==(const Location& other) const {
        return other.row == row && other.col == col;
    }

    bool operator!=(const Location& other) const {
        return other.row != row || other.col != col;
    }
};

class Node {
public:
    Node(std::string name, std::string value) : id(name), val(value) { }
    Node(std::string name, std::string value, Location loc) : Node(name, value) { location = loc; }
    std::string getID() {return id;}
    std::string getVal() {return val;}
    std::vector<Node*> getEdges() {return edges;}

    void addEdge(Node* newEdge) {edges.push_back(newEdge);}
    void clearEdges();
    void printEdges(int numTabs);

    std::string id;
    std::string val;
    Location location = Location::NONE;
    std::vector<Node*> edges;
};



#endif /*NODE_H*/