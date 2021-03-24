#ifndef ASTARALGORITHM_H
#define ASTARALGORITHM_H

#include "types.h"
#include <vector>
#include <algorithm>
#include <deque>
#include <unordered_set>
#include <list>
#include <functional>
#include "staticpfalgorithm.h"

class AstarAlgorithm :public StaticPFAlgorithm
{
public:
    AstarAlgorithm();
    virtual ~AstarAlgorithm();
    virtual void setMap(unsigned _x, unsigned _y, const std::vector<MPoint> &_blockVec);
    virtual void run(const MPoint & _s, const MPoint & _e);
    virtual bool isValid(MAstarPoint * _l, MAstarPoint * _n);
    virtual bool isValid(const int _x, const int _y);

public:
    static const unsigned vCost = 100;
    static const unsigned hCost = 100;
    static const unsigned dCost = 141;

protected:

    MAstarPoint * findPath(const MPoint & _s, const MPoint & _e);
    MAstarPoint * getLeastFpoint();
    unsigned isInOpenList(MAstarPoint * _p);
    void refreshOpenList(unsigned _subscript);
    void pushInOpenList(MAstarPoint *_p);

    virtual void getNeighbour(MAstarPoint * _p);
    virtual void getNext(MAstarPoint * _p);
    virtual void generatePath(MAstarPoint * _r);
    unsigned calF(MAstarPoint * _p);
    virtual unsigned calH(MAstarPoint * _p, MAstarPoint * _e);
    virtual unsigned calG(MAstarPoint * _p, MAstarPoint * _l);

    virtual std::vector<MPoint> & getInfo1List();
    virtual std::vector<MPoint> & getInfo2List();


protected:
    //std::vector <std::vector <> >
    std::vector<MAstarPoint *> mOpenList;
    std::list <MAstarPoint *> mCloseList;
    std::vector < std::vector <MAstarPoint> > mAstarMap;

    std::vector <MAstarPoint *> mSuccessors;

    MAstarPoint * mStart;
    MAstarPoint * mEnd;

    bool mDiagVaild;
};


#endif // ASTARALGORITHM_H
