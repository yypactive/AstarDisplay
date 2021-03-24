#ifndef GBOPTIMIZATION_H
#define GBOPTIMIZATION_H

#include "dijkstranode.h"
#include "jpsplusalgorithm.h"
#include "types.h"
#include <map>
#include <iostream>

class GBOptimization : public JPSPlusAlgorithm
{
public:
    GBOptimization();
    virtual ~GBOptimization() {}

    virtual void setMap(unsigned _x, unsigned _y, const std::vector <MPoint> & _blockVec);

protected:
    virtual void setGBInfoMap();
    virtual void getIdentifySuccessors(MAstarPoint * _p);

private:
    bool isInGB(MPoint * _l, short _dir);
    void initDijkstraMap();
    void dijkstraFlood(unsigned _x, unsigned _y);
    void explore(DijkstraNode * _node, short _dir , bool isStart = false);
    unsigned calG(DijkstraNode * _n, DijkstraNode * _l);

protected:
    std::vector <std::vector <MGBInfo> > mGBInfoMap;
    std::map <std::pair<short, short>, short> mPair2DirHash;

    unsigned mGBIteration;
    std::vector < std::vector< DijkstraNode > > mDijkstraMap;
    std::vector <DijkstraNode * > mDijkstraList;

};

#endif // GBOPTIMIZATION_H
