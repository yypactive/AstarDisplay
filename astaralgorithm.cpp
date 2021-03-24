  #include "astaralgorithm.h"
#include <cmath>
#include <windows.h>
#include <stack>

AstarAlgorithm::AstarAlgorithm()
    : mOpenList(0),
      mCloseList(0),
      mAstarMap(0),
      mStart(NULL),
      mEnd(NULL),
      mDiagVaild(true)
{

}

AstarAlgorithm::~AstarAlgorithm()
{

}

void AstarAlgorithm::setMap(unsigned _x, unsigned _y, const std::vector<MPoint> &_blockVec)
{
    PathFindingAlgorithm::setMap(_x, _y, _blockVec);
    mAstarMap.resize(mMap.size());
    for (unsigned i = 0; i < mMap.size(); ++i)
    {
        mAstarMap[i].resize(mMap[0].size());
        for (unsigned j = 0; j < mMap[0].size(); ++j)
            mAstarMap[i][j] = MAstarPoint(j, i);
    }
    mOpenList.clear();
    mCloseList.clear();
}

void AstarAlgorithm::run(const MPoint & _s, const MPoint & _e)
{
    if (!mMapValid)
        return;
    mVacantValid = false;
    mTimeRecord = 0;
    for (auto it = mCloseList.begin(); it != mCloseList.end(); it++)
    {
        (*it)->isClose = false;
    }
    for (auto it = mOpenList.begin(); it != mOpenList.end(); it++)
    {
        (*it)->isOpen = false;
    }
    mOpenList.clear();
    mCloseList.clear();
    LARGE_INTEGER t1,t2,tc;
    QueryPerformanceFrequency(&tc);
    QueryPerformanceCounter(&t1);
    auto res = findPath(_s, _e);
    QueryPerformanceCounter(&t2);
    if (res)
    {
        generatePath(res);
        mResultValid = true;
    }
    else
    {
        mResultValid = false;
    }
    mTimeRecord = (t2.QuadPart - t1.QuadPart) * 1.0 / tc.QuadPart;
    mVacantValid = true;
}

MAstarPoint * AstarAlgorithm::findPath(const MPoint & _s, const MPoint & _e)
{
    if (_s.x < 0 || _s.x >= mMap[0].size() || _s.y < 0 || _s.y >= mMap.size())
        return NULL;
    mStart = &mAstarMap[_s.y][_s.x];
    mStart->G = 0;
    mStart->H = 0;
    mStart->F = 0;
    mEnd = &mAstarMap[_e.y][_e.x];
    mOpenList.push_back(mStart);
    while (!mOpenList.empty())
    {
        auto nowPoint = getLeastFpoint();
        mCloseList.push_back(nowPoint);
        nowPoint->isClose = true;
        getNext(nowPoint);

        for (unsigned i = 0; i < mSuccessors.size(); ++i)
        {
            auto &target = mSuccessors[i];
            if (!target->isOpen)
            {
                target->father = nowPoint;
                target->G = calG(nowPoint, target);
                target->H = calH(target, mEnd);
                target->F = calF(target);
                pushInOpenList(target);
            }
            else
            {
                int tmpG = calG(nowPoint, target);
                if (tmpG < target->G)
                {
                    target->father = nowPoint;
                    target->G = tmpG;
                    target->F = calF(target);
                    unsigned sub = isInOpenList(target);
                    refreshOpenList(sub);
                }
            }
        }
        if (mEnd->isClose)
            return mEnd;
    }
    return NULL;
}

MAstarPoint * AstarAlgorithm::getLeastFpoint()
{
    /*if(!mOpenList.empty())
    {
        auto res = mOpenList.front();
        for (auto it = mOpenList.begin(); it != mOpenList.end(); ++it)
        {
            if((*it)->F < res->F)
                res = (*it);
        }
        return res;
    }
    return NULL;*/
    if (!mOpenList.empty())
    {
        auto p = mOpenList.front();
        std::pop_heap (mOpenList.begin(), mOpenList.end(), cmpF());
        mOpenList.pop_back();
        p->isOpen = false;
        return p;
    }
    return NULL;

}

unsigned AstarAlgorithm::isInOpenList(MAstarPoint * _p)
{
    unsigned i = 0;
    unsigned size = mOpenList.size();
    while (i < size)
    {
        if (mOpenList[i] == _p)
            return i;
        i++;
    }
    return i;
    /*
    if (mOpenList.empty())
        return 0;

    unsigned current = 0;
    unsigned size = mOpenList.size();
    std::stack < unsigned > myStack;
    myStack.push(current);
    while (!myStack.empty())
    {
        current = myStack.top();
        myStack.pop();
        if (mOpenList[current] == _p)
            return current;
        else if (mOpenList[current]->F <= _p->F)
        {
            if ((current << 1) + 1 < size)
                myStack.push((current << 1) + 1);
            if ((current << 1) + 2 < size)
                myStack.push((current << 1) + 2);
        }
    }
    return size;
    */
}

void AstarAlgorithm::refreshOpenList(unsigned _subscript)
{
    //std::make_heap(mOpenList.begin(), mOpenList.end(), cmpF());
    //dangerous
    unsigned current = _subscript;
    //unsigned size = mOpenList.size();
    unsigned father = ((current + 1) >> 1) - 1;
    // unsigned leftson = (current << 1) + 1;
    // unsigned rightson = (current << 1) + 2;
    if (current > 0 && mOpenList[father]->F > mOpenList[current]->F)
    {
        do
        {
            std::swap(mOpenList[current], mOpenList[father]);
            current = father;
            father = ((current + 1) >> 1) - 1;
        } while (current > 0 && mOpenList[father]->F > mOpenList[current]->F);
        return;
    }
/*
    else if (leftson < size)
    {
        do
        {
            if (rightson < size)
            {
                unsigned smallson = mOpenList[leftson]->F < mOpenList[rightson]->F ? leftson : rightson;
                unsigned bigson = smallson == leftson ? rightson : leftson;
                if (mOpenList[current]->F > mOpenList[smallson]->F)
                {
                    std::swap(mOpenList[current], mOpenList[smallson]);
                    current = smallson;
                    leftson = (current << 1) + 1;
                    rightson = (current << 1) + 2;
                }
                else
                {
                    return;
                }
            }
            else // only have left son
            {
                if (mOpenList[current]->F > mOpenList[leftson]->F)
                {
                    std::swap(mOpenList[current], mOpenList[leftson]);
                }
                return;
            }
        } while (leftson < size);
        return;
    }
    */
}

void AstarAlgorithm::pushInOpenList(MAstarPoint * _p)
{
     mOpenList.push_back(_p);
     std::push_heap (mOpenList.begin(),mOpenList.end(), cmpF());
     _p->isOpen = true;
}

void AstarAlgorithm::getNeighbour(MAstarPoint * _p)
{
    mSuccessors.clear();
    for (int i = _p->y - 1; i < _p->y + 2; ++i)
    {
        for (int j = _p->x - 1; j < _p->x + 2; ++j)
        {
            if (isValid(j, i))
                if (isValid(_p, &mAstarMap[i][j]))
                    mSuccessors.push_back(&mAstarMap[i][j]);
        }
    }
}

void AstarAlgorithm::getNext(MAstarPoint * _p)
{
    getNeighbour(_p);
}

bool AstarAlgorithm::isValid(MAstarPoint * _l, MAstarPoint * _n)
{
    if (!isValid(_n->x, _n->y))
        return false;
//    if (mMap[_n->y][_n->x] == 1)
//        return false;
    if (isblocked(_n->x, _n->y))
        return false;
//    auto it = mCloseList.find(&mAstarMap[_n->y][_n->x]);
//    if (it != mCloseList.end())
    if (_n->isClose)
        return false;
    if(abs(_n->x - _l->x) + abs(_n->y - _l->y) < 2)
        return true;
    //if(mMap[_n->y][_l->x]== 1 && mMap[_l->y][_n->x] == 1)
        //return false;
    else
        return mDiagVaild;
}

bool AstarAlgorithm::isValid(const int _x, const int _y)
{
    if (mMap.empty())
        return false;
    if (_x < 0 || _x >= sizeX)
        return false;
    if (_y < 0 || _y >= sizeY)
        return false;
    return true;
}

void AstarAlgorithm::generatePath(MAstarPoint * _r)
{
    mStart->father = NULL;
    mPathQue.clear();
    for (auto p = _r; p != NULL; p = p->father)
    {
        mPathQue.push_front(MPoint(p->x, p->y));
    }
}

unsigned AstarAlgorithm::calF(MAstarPoint * _p)
{
    return _p->G + _p->H;
}

unsigned AstarAlgorithm::calH(MAstarPoint * _p, MAstarPoint * _e)
{
    unsigned dx = abs(_p->x - _e->x);
    unsigned dy = abs(_p->y - _e->y);
    unsigned dd = dx < dy ? dx : dy;
    dx -= dd;
    dy -= dd;
    return dx * hCost + dy * vCost + dd * dCost;
}
unsigned AstarAlgorithm::calG(MAstarPoint * _p, MAstarPoint * _l)
{
    unsigned deltaG;
    if (_p->x == _l->x)
        deltaG = vCost * abs(_p->y - _l->y);
    else if (_p->y == _l->y)
        deltaG = hCost * abs(_p->x - _l->x);
    else
        deltaG = dCost * abs(_p->x - _l->x);
    //unsigned fatherG = _p->father ? _p->father->G : 0;
    unsigned fatherG = _p ? _p->G : 0;
    return fatherG + deltaG;
}

std::vector<MPoint> & AstarAlgorithm::getInfo1List()
{
    mInfo1Vec.clear();
    for (auto it = mOpenList.begin(); it != mOpenList.end(); it++)
    {
        mInfo1Vec.push_back(MPoint((*it)->x, (*it)->y));
    }
    return mInfo1Vec;
}

std::vector<MPoint> & AstarAlgorithm::getInfo2List()
{
    mInfo2Vec.clear();
    for (auto it = mCloseList.begin(); it != mCloseList.end(); it++)
    {
        mInfo2Vec.push_back(MPoint((*it)->x, (*it)->y));
    }
    return mInfo2Vec;
}
