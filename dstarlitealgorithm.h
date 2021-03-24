#ifndef DSTARLITEALGORITHM_H
#define DSTARLITEALGORITHM_H

#include "types.h"
#define NOMINMAX
#include <algorithm>
#include <windows.h>
#include <deque>
#include <unordered_map>
//#include <QDebug>
#include <list>
#include <functional>
#include <cmath>
#include <iostream>
#include "pathfindingalgorithm.h"

struct cmpkey
{
    bool operator()(MDstarLitePoint * a, MDstarLitePoint * b)
    {
        return a->key > b->key;
    }
};

class DstarLiteAlgorithm : public PathFindingAlgorithm
{
public:
    DstarLiteAlgorithm();
    virtual ~DstarLiteAlgorithm() {}

    virtual void setMap(unsigned _x, unsigned _y, const std::vector<MPoint> &_blockVec);
    virtual void run(const MPoint & _s, const MPoint & _e);
    virtual void rerun(const MPoint & _rs, const MPoint & _e);
    virtual void changePoint(const MPoint & _s, int _block);

public:
    static const unsigned vCost = 10;
    static const unsigned hCost = 10;
    static const unsigned dCost = 14;

protected:
    void initalize(MPoint _s, MPoint _e);
    void updatePoint(MDstarLitePoint *_p);
    void refreshCost();
    MDstarLitePoint * findPath(const MPoint & _s, const MPoint & _e);
    void generatePath(MDstarLitePoint *_r);

    void succ(const MDstarLitePoint * _p);
    void pred(const MDstarLitePoint * _p);
    bool isValid(const int _x, const int _y);

    MDstarLitePoint * getLeastKeyPoint();
    unsigned isInOpenList(MDstarLitePoint * _p);
    void pushOpenList(MDstarLitePoint * _p);
    void refreshOpenList(unsigned _subscript);
    void removeFromOpenList(MDstarLitePoint * _p);

    unsigned cost(MDstarLitePoint * _u, MDstarLitePoint * _v);
    unsigned calH(MDstarLitePoint *_p, MDstarLitePoint *_e);
    unsigned plus(unsigned _x, unsigned _y);
    unsigned cplusg(MDstarLitePoint *_f, MDstarLitePoint *_s);
    unsigned calrhs(MDstarLitePoint * _p);
    std::pair<unsigned, unsigned> calKey(MDstarLitePoint * _p);

protected:
    unsigned mKm;

    std::vector < std::vector <MDstarLitePoint> > mDstarLiteMap;

    MDstarLitePoint * mStart;
    MDstarLitePoint * mLast;
    MDstarLitePoint * mEnd;

    std::vector<MDstarLitePoint * > mOpenList;
    std::list<MDstarLitePoint * > mSuccList;
    std::list<MDstarLitePoint * > mPredList;
    std::unordered_map <MPoint , std::pair<int, int>, HashPoint, EqualPoint> mChangePointMap;
    std::unordered_map <std::pair<MDstarLitePoint * , MDstarLitePoint * >, unsigned, pairhash> mCostMap;
    bool mDiagVaild;

};

#endif // DSTARLITEALGORITHM_H
