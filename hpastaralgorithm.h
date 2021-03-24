#ifndef HPASTARALGORITHM_H
#define HPASTARALGORITHM_H

#include "types.h"
#include "astaralgorithm.h"
#include <list>
#include <windows.h>
#include <unordered_map>

struct Cluster;
struct Entrance;
struct Edge;

struct MHPAstarInfo
{
    unsigned x;
    unsigned y;
    Cluster * cluster;
    Entrance * entrance;
    short level;
    std::list < unsigned > edges;

    MHPAstarInfo()
        : x(0), y(0), cluster(NULL), entrance(NULL), level(0), edges(0)
    {

    }

    MHPAstarInfo(unsigned _x, unsigned _y, Cluster * _c,
                 Entrance * _e, short _l)
        : x(_x), y(_y), cluster(_c),
          entrance(_e), level(_l), edges(0)
    {

    }
};

struct Cluster
{
    unsigned ltx;
    unsigned lty;
    unsigned sx;
    unsigned sy;
    short level;
    std::list <MHPAstarInfo *> nodes;
    std::list <MHPAstarInfo *> upnodes;
    std::list <MHPAstarInfo *> leftnodes;
    std::list <MHPAstarInfo *> rightnodes;
    std::list <MHPAstarInfo *> downnodes;

    Cluster()
        : ltx(0), lty(0), sx(0), sy(0), level(0)
    {

    }

    Cluster(unsigned _x, unsigned _y, unsigned _sx, unsigned _sy, short _l)
        : ltx(_x), lty(_y), sx(_sx), sy(_sy), level(_l)
    {

    }

    bool inIt(unsigned _x, unsigned _y)
    {
        return ltx <= _x && ltx + sx > _x && lty <= _y && lty + sy > _y;
    }
};

struct Entrance
{
    MHPAstarInfo *n1;
    MHPAstarInfo *n2;
    Cluster *c1;
    Cluster *c2;
    unsigned edge;
    bool isInter;
    short level;

    Entrance()
        : n1(NULL), n2(NULL), c1(NULL), c2(NULL), edge(NULL), isInter(true), level(1)
    {

    }

    Entrance(
            MHPAstarInfo * _n1, MHPAstarInfo * _n2,
            Cluster * _c1, Cluster *_c2,
            bool _inInter, short _level)
        : n1(_n1), n2(_n2), c1(_c1), c2(_c2), edge(NULL),
          isInter(_inInter), level(_level)
    {

    }

};

struct Edge
{
    MHPAstarInfo * s;
    MHPAstarInfo * d;
    short level;
    unsigned cost;
    bool isInter;

    Edge(
            MHPAstarInfo * _s, MHPAstarInfo * _d,
            short _l, unsigned _c, bool _isInter)
        : s(_s), d(_d), level(_l), cost(_c),
          isInter(_isInter)
    {

    }
};

class HPAstarAlgorithm : public AstarAlgorithm
{
public:
    HPAstarAlgorithm();
    virtual ~HPAstarAlgorithm() {}
    virtual void setMap(unsigned _x, unsigned _y, const std::vector <MPoint> & _blockVec);
    virtual void run(const MPoint & _s, const MPoint & _e);

    virtual std::vector<MPoint> & getInfo1List();
    virtual std::vector<MPoint> & getInfo2List();

protected:
    virtual void getNext(MAstarPoint * _p);
    virtual bool isValid(const int _x, const int _y);
    virtual void generatePath(MAstarPoint * _r);
    virtual unsigned calG(MAstarPoint * _p, MAstarPoint * _l);

    //pre-processing
    void preprocessing();
    void abstractMaze();
    void buildGraph();
    void addLevelToGraph(const unsigned _i);
    void buildCluster(short _i);
    void buildEntrances(Cluster *_c1, Cluster *_c2, short _dir);
    int searchforDistance(const MPoint & _s, const MPoint & _e, Cluster *_c);
    void insertNode(const MPoint & _s, short _maxl);
    void connectToBorder(const MPoint & _s, Cluster * c);
    void eraseNode(const MPoint & _s, const MPoint &_e);

    // on-line search
    MAstarPoint * onlineSearch(const MPoint & _s, const MPoint & _e);
    void refinePath(short _maxl);
    void smoothPath();

public:
    static const unsigned mMaxlevel = 2;
    static const unsigned mMinClusterW = 16;
    static const unsigned mMinEW = 4;

protected:
    std::vector < Entrance > mIntrList;
    std::vector < Edge > mEdgeList;
    Cluster mMaxCluster;
    Cluster * mCurrentCluster;
    short mLevel;
    unsigned mEdgeSize;
    std::vector < Cluster * > mDirtyClustersVec;
    std::unordered_map < MAstarPoint *, unsigned > mCostHash;
    std::vector < std::vector < Cluster > > mClustersVec;
    std::vector < std::vector < MHPAstarInfo > > mHPAstarInfoMap;

};

#endif // HPASTARALGORITHM_H
