#include "dstarlitealgorithm.h"
#include <windows.h>
DstarLiteAlgorithm::DstarLiteAlgorithm()
    : mKm(0),
      mDstarLiteMap(0),
      mStart(NULL),
      mEnd(NULL),
      mOpenList(0),
      mSuccList(0),
      mPredList(0)
{
}

void DstarLiteAlgorithm::setMap(unsigned _x, unsigned _y, const std::vector<MPoint> &_blockVec)
{
    PathFindingAlgorithm::setMap(_x, _y, _blockVec);
    mDstarLiteMap.resize(sizeY);
    for (unsigned i = 0; i < sizeY; ++i)
        mDstarLiteMap[i].resize(sizeX);

}

void DstarLiteAlgorithm::run(const MPoint & _s, const MPoint & _e)
{
    if (!mMapValid)
        return;
    mVacantValid = false;
    mOpenList.clear();
    mTimeRecord = 0;
    for (unsigned i = 0; i < sizeY; ++i)
    {
        for (unsigned j = 0; j < sizeX; ++j)
            mDstarLiteMap[i][j] = MDstarLitePoint(j, i);
    }
    if (_s.x < 0 || _s.x >= mMap[0].size() || _s.y < 0 || _s.y >= mMap.size())
        return;
    initalize(_s, _e);
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
    //qDebug() << "time: " << mTimeRecord;
    mVacantValid = true;
}

void  DstarLiteAlgorithm::rerun(const MPoint & _rs, const MPoint &_e)
{
    mVacantValid = false;
 /*   for (auto it = mChangePointMap.begin(); it != mChangePointMap.end(); ++it)
    {
        qDebug() << "x: " << it->first.x << "y: " << it->first.y << "old: " << it->second.first << it->second.second;
    }*/

    LARGE_INTEGER t1,t2,tc;
    QueryPerformanceFrequency(&tc);
    QueryPerformanceCounter(&t1);
    if (mChangePointMap.empty())
    {
        QueryPerformanceCounter(&t2);
        mTimeRecord = (t2.QuadPart - t1.QuadPart) * 1.0 / tc.QuadPart;
        mVacantValid = true;
        return;
    }
    mStart = &mDstarLiteMap[_rs.y][_rs.x];
    mKm += calH(mLast, mStart);
    mLast = mStart;
    refreshCost();
 /*   for (unsigned j = 0; j < mMap.size(); ++j)
    {
        QDebug deb = qDebug();
        for (unsigned i = 0; i< mMap.size(); ++i)
        {
            deb.nospace() << (int)(mDstarLiteMap[j][i].rhs) << "\t";
        }
    }*/
    auto res = findPath(_rs, _e);
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
    mChangePointMap.clear();
    mVacantValid = true;
}

void  DstarLiteAlgorithm::changePoint(const MPoint & _s, int _block)
{
    if (mMap[_s.y][_s.x] == _block)
        return;
    auto it = mChangePointMap.find(_s);
    if (it != mChangePointMap.end())
    {
        if (it->second.first == _block)
            mChangePointMap.erase(it);
        else
        {
           it->second.second = _block;
        }
    }
    else
    {
        mChangePointMap[_s] = std::make_pair(mMap[_s.y][_s.x], _block);
    }

}

void DstarLiteAlgorithm::generatePath(MDstarLitePoint * _r)
{
    mPathQue.clear();
    for (auto p = _r; p != NULL; p = p->father)
    {
        mPathQue.push_back(MPoint(p->x, p->y));
    }
}


void DstarLiteAlgorithm::initalize(MPoint _s, MPoint _e)
{
    if (!mMapValid)
        return;
    mPathQue.clear();
    mOpenList.clear();
    mKm = 0;
    mStart = &mDstarLiteMap[_s.y][_s.x];
    mEnd = &mDstarLiteMap[_e.y][_e.x];
    mEnd->rhs = 0;
    mEnd->H = calH(mStart, mEnd);
    mEnd->key = calKey(mEnd);
    mOpenList.push_back(mEnd);
    mLast = mStart;
}

void DstarLiteAlgorithm::updatePoint(MDstarLitePoint * _p)
{
    if (_p->G != _p->rhs)
    {
        if (_p->isOpen)
        {
            _p->H = calH(_p, mStart);
            _p->key = calKey(_p);
            unsigned sub = isInOpenList(_p);
            refreshOpenList(sub);
        }
        else
        {
            _p->H = calH(_p, mStart);
            _p->key = calKey(_p);
            pushOpenList(_p);
        }
    }
    else
    {
        if (_p->isOpen)
        {
            removeFromOpenList(_p);
        }
    }
}

void DstarLiteAlgorithm::refreshCost()
{
    mCostMap.clear();
    for (auto it = mChangePointMap.begin(); it != mChangePointMap.end(); it++)
    {
        auto p = it->first;
        pred(&mDstarLiteMap[p.y][p.x]);
        for (auto pre = mPredList.begin(); pre != mPredList.end(); ++pre)
            mCostMap[std::make_pair(*pre, &mDstarLiteMap[p.y][p.x])] = cost(*pre, &mDstarLiteMap[p.y][p.x]);
    }
    for (auto it = mChangePointMap.begin(); it != mChangePointMap.end(); it++)
    {
        PathFindingAlgorithm::changePoint(it->first, it->second.second);
    }
    for (auto it = mChangePointMap.begin(); it != mChangePointMap.end(); it++)
    {
        auto p = it->first;
        succ(&mDstarLiteMap[p.y][p.x]);
        for (auto suc = mSuccList.begin(); suc != mSuccList.end(); ++suc)
            mCostMap[std::make_pair(&mDstarLiteMap[p.y][p.x], *suc)] = cost(*suc, &mDstarLiteMap[p.y][p.x]);
    }
    for (auto it = mCostMap.begin(); it != mCostMap.end(); ++it)
    {
        auto upoint = it->first.first;
        auto vpoint = it->first.second;
        auto cold = it->second;
        auto oldcplusg = plus(cold, vpoint->G);
        if (cold > cost(upoint, vpoint))
        {
            if (upoint != mEnd)
            {
                unsigned newrhs = plus(cost(upoint, vpoint), vpoint->G);
                if (newrhs < upoint->rhs)
                {
                    upoint->father = vpoint;
                    upoint->rhs = newrhs;
                }
            }
        }
        else if (upoint->rhs == oldcplusg)
        {
            if (upoint != mEnd)
            {
                upoint->rhs = calrhs(upoint);
            }
        }
        updatePoint(upoint);
    }
}

MDstarLitePoint * DstarLiteAlgorithm::findPath(const MPoint & _s, const MPoint & _e)
{
     while (!mOpenList.empty())
    {
        if (getLeastKeyPoint()->key >= calKey(mStart) && mStart->rhs <= mStart->G)
            break;
        auto u = getLeastKeyPoint();
        auto kold = u->key;
        u->H = calH(u, mStart);
        auto knew = calKey(u);
        if (kold < knew)
        {
            u->key = knew;
            unsigned sub = isInOpenList(u);
            refreshOpenList(sub);
        }
        else if (u->G > u->rhs)
        {
            u->G = u->rhs;
            removeFromOpenList(u);
            pred(u);
            for (auto s = mPredList.begin(); s != mPredList.end(); ++s)
            {
                if (*s != mEnd)
                {
                    unsigned newrhs = cplusg((*s), u);
                    if (newrhs < (*s)->rhs)
                    {
                        (*s)->father = u;
                        (*s)->rhs = newrhs;
                    }
                }
                updatePoint(*s);
            }
        }
        else
        {
            auto gold = u->G;
            u->G = std::numeric_limits<unsigned>::max();
            pred(u);
            mPredList.push_back(u);
            for (auto s = mPredList.begin(); s != mPredList.end(); ++s)
            {
                auto cplusgold = plus(cost((*s), u), gold);
                if ((*s)->rhs == cplusgold)
                {

                    if ((*s) != mEnd)
                    {
                        (*s)->rhs = calrhs(*s);
                    }
                }
                updatePoint(*s);
            }
        }
    }
    if (mStart->rhs == std::numeric_limits<unsigned>::max())
        return NULL;
    else
        return mStart;
}

void DstarLiteAlgorithm::succ(const MDstarLitePoint * _p)
{
    mSuccList.clear();
    if (mMap[_p->y][_p->x] == 1) return;
    for (int i = _p->y - 1; i < _p->y + 2; ++i)
        for (int j = _p->x - 1; j < _p->x + 2; ++j)
        {
            if (isValid(j ,i) && ( i != _p->y || j != _p->x ))
                mSuccList.push_front(&mDstarLiteMap[i][j]);
        }
}

void DstarLiteAlgorithm::pred(const MDstarLitePoint * _p)
{
    mPredList.clear();
    for (int i = _p->y - 1; i < _p->y + 2; ++i)
        for (int j = _p->x - 1; j < _p->x + 2; ++j)
        {
            if (isValid(j ,i) && ( i != _p->y || j != _p->x ))
                if (mMap[i][j] == 0)
                    mPredList.push_front(&mDstarLiteMap[i][j]);
        }
}

bool DstarLiteAlgorithm::isValid(const int _x, const int _y)
{
    if (mMap.empty())
        return false;
    if (_x < 0 || _x >= sizeX)
        return false;
    if (_y < 0 || _y >= sizeY)
        return false;
    return true;
}

MDstarLitePoint * DstarLiteAlgorithm::getLeastKeyPoint()
{
    if (!mOpenList.empty())
    {
        auto p = mOpenList.front();
        return p;
    }
    else
        return NULL;
}

unsigned DstarLiteAlgorithm::isInOpenList(MDstarLitePoint * _p)
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
}

void DstarLiteAlgorithm::pushOpenList(MDstarLitePoint * _p)
{
    mOpenList.push_back(_p);
    std::push_heap (mOpenList.begin(),mOpenList.end(), cmpkey());
    _p->isOpen = true;
}

void DstarLiteAlgorithm::refreshOpenList(unsigned _subscript)
{
    //std::make_heap(mOpenList.begin(), mOpenList.end(), cmpF());
    //dangerous
    unsigned current = _subscript;
    unsigned size = mOpenList.size();
    unsigned father = ((current + 1) >> 1) - 1;
    unsigned leftson = (current << 1) + 1;
    unsigned rightson = (current << 1) + 2;
    if (current > 0 && mOpenList[father]->key > mOpenList[current]->key)
    {
        do
        {
            std::swap(mOpenList[current], mOpenList[father]);
            current = father;
            father = ((current + 1) >> 1) - 1;
        } while (current > 0 && mOpenList[father]->key > mOpenList[current]->key);
        return;
    }
    else if (leftson < size)
    {
        do
        {
            if (rightson < size)
            {
                unsigned smallson = mOpenList[leftson]->key < mOpenList[rightson]->key ? leftson : rightson;
                if (mOpenList[current]->key > mOpenList[smallson]->key)
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
                if (mOpenList[current]->key > mOpenList[leftson]->key)
                {
                    std::swap(mOpenList[current], mOpenList[leftson]);
                }
                return;
            }
        } while (leftson < size);
        return;
    }
}

void DstarLiteAlgorithm::removeFromOpenList(MDstarLitePoint * _p)
{
    unsigned current;
    for (current = 0; current < mOpenList.size(); current++)
    {
        if (mOpenList[current] == _p)
            break;
    }
    if (current >= mOpenList.size())
        return;
    unsigned father = ((current + 1) >> 1) - 1;
    while (current > 0)
    {
        std::swap(mOpenList[current], mOpenList[father]);
        current = father;
        father = ((current + 1) >> 1) - 1;
    }
    mOpenList.front()->isOpen = false;
    std::pop_heap (mOpenList.begin(),mOpenList.end(), cmpkey());
    mOpenList.pop_back();
}

unsigned DstarLiteAlgorithm::cost(MDstarLitePoint *_p, MDstarLitePoint *_l)
{
    if (mMap[_p->y][_p->x] == 1 || mMap[_l->y][_l->x] == 1)
        return std::numeric_limits<unsigned>::max();
    unsigned deltaG;
    if (_p->x == _l->x)
        deltaG = vCost;
    else if (_p->y == _l->y)
        deltaG = hCost;
    else
        deltaG = dCost;
    return deltaG;
}

unsigned DstarLiteAlgorithm::calH(MDstarLitePoint * _p, MDstarLitePoint * _e)
{
    unsigned dx = abs(_p->x - _e->x);
    unsigned dy = abs(_p->y - _e->y);
    unsigned dd = std::min(dx, dy);
    dx -= dd;
    dy -= dd;
    return dx * hCost + dy * vCost + dd * dCost;
}

unsigned DstarLiteAlgorithm::plus(unsigned _x, unsigned _y)
{
    if (_x == std::numeric_limits<unsigned>::max() || _y == std::numeric_limits<unsigned>::max())
        return std::numeric_limits<unsigned>::max();
    else
        return (_x + _y);
}

unsigned DstarLiteAlgorithm::cplusg(MDstarLitePoint *_f, MDstarLitePoint *_s)
{
    unsigned tmp1 = cost(_f, _s);
    unsigned tmp2 = _s->G;
    if (tmp1 == std::numeric_limits<unsigned>::max() || tmp2 == std::numeric_limits<unsigned>::max())
        return std::numeric_limits<unsigned>::max();
    else
        return (tmp1 + tmp2);
}

unsigned DstarLiteAlgorithm::calrhs(MDstarLitePoint *_f)
{
    //if (_f == mStart)
        //return 0;
    unsigned minrhs = std::numeric_limits<unsigned>::max();
    succ(_f);
    for (auto n = mSuccList.begin(); n != mSuccList.end(); ++n)
    {
        auto nowrhs = cplusg(_f, *n);
        if (minrhs > nowrhs)
        {
            minrhs = nowrhs;
            _f->father = (*n);
        }
    }
    return minrhs;
}

std::pair<unsigned, unsigned> DstarLiteAlgorithm::calKey(MDstarLitePoint * _p)
{
    return std::make_pair(plus(std::min(_p->G, _p->rhs), (_p->H + mKm)), std::min(_p->G, _p->rhs));
}
