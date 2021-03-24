#ifndef DIJKSTRANODE_H
#define DIJKSTRANODE_H

struct DijkstraNode
{
public:
    DijkstraNode();

    enum ListStatus
    {
        OnNone,
        OnOpen,
        OnClosed
    };

public:
    unsigned row;
    unsigned col;
    DijkstraNode* parent;
    unsigned int givenCost;
    unsigned int equalCost;
    unsigned int iteration;
    unsigned char dirFromStart;
    unsigned char dirFromParent;
    unsigned char listStatus;
};

struct cmpGivenCost
{
    bool operator()(DijkstraNode * a, DijkstraNode * b)
    {
        return a->givenCost > b->givenCost;
    }
};

struct cmpEqualCost
{
    bool operator()(DijkstraNode * a, DijkstraNode * b)
    {
        return a->equalCost > b->equalCost;
    }
};

#endif // DIJKSTRANODE_H
