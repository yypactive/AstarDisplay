#include "gboptimization.h"
#include <QDebug>
#include <fstream>

GBOptimization::GBOptimization()
    : mGBIteration(1)
{
    mPair2DirHash[std::make_pair(1, 1)] = MJPSInfo::downright;
    mPair2DirHash[std::make_pair(0, 1)] = MJPSInfo::down;
    mPair2DirHash[std::make_pair(-1, 1)] = MJPSInfo::downleft;
    mPair2DirHash[std::make_pair(1, 0)] = MJPSInfo::right;
    mPair2DirHash[std::make_pair(-1, 0)] = MJPSInfo::left;
    mPair2DirHash[std::make_pair(1, -1)] = MJPSInfo::upright;
    mPair2DirHash[std::make_pair(0, -1)] = MJPSInfo::up;
    mPair2DirHash[std::make_pair(-1, -1)] = MJPSInfo::upleft;
}

void GBOptimization::setMap(unsigned _x, unsigned _y, const std::vector <MPoint> & _blockVec)
{
    JPSPlusAlgorithm::setMap(_x, _y, _blockVec);
    mGBInfoMap.resize(sizeY);
    for (unsigned y = 0; y < sizeY; ++y)
        mGBInfoMap[y].resize(sizeX);
    qDebug() << "filename" << QString::fromStdString(mFileName);

    if (mFileName != "")
    {
        std::string gbfile = mFileName + "_gb";
        std::ifstream fin(gbfile.c_str());
        if (!fin)
        {
            setGBInfoMap();
        }
        else
        {
            for (unsigned y = 0; y < sizeY; ++y)
            {
                for (unsigned x = 0; x < sizeX; ++x)
                {
                    for (short i = 0; i < 8; ++i)
                    {
                        for (short j = 0; j < 4; ++j)
                        {
                            fin >> mGBInfoMap[y][x].bounds[i][j];
                        }
                    }
                }
            }
        }
        fin.close();
    }
    else
    {
        setGBInfoMap();
    }
#if 0
    qDebug() << "right";
    for (unsigned j = 0; j < sizeY; ++j)
    {
        QDebug deb = qDebug();
        for (unsigned i = 0; i< sizeX; ++i)
            deb.nospace() << (int)(mGBInfoMap[j][i].bounds[MJPSInfo::right][MGBInfo::minrow])
                    << " " << (int)(mGBInfoMap[j][i].bounds[MJPSInfo::right][MGBInfo::maxrow]) << "\t";
        deb = qDebug();
        for (unsigned i = 0; i< sizeX; ++i)
            deb.nospace() << (int)(mGBInfoMap[j][i].bounds[MJPSInfo::right][MGBInfo::mincol])
                    << " " << (int)(mGBInfoMap[j][i].bounds[MJPSInfo::right][MGBInfo::maxcol]) << "\t";
    }
    qDebug() << "downright";
    for (unsigned j = 0; j < sizeY; ++j)
    {
        QDebug deb = qDebug();
        for (unsigned i = 0; i< sizeX; ++i)
            deb.nospace() << (int)(mGBInfoMap[j][i].bounds[MJPSInfo::downright][MGBInfo::minrow])
                    << " " << (int)(mGBInfoMap[j][i].bounds[MJPSInfo::downright][MGBInfo::maxrow]) << "\t";
        deb = qDebug();
        for (unsigned i = 0; i< sizeX; ++i)
            deb.nospace() << (int)(mGBInfoMap[j][i].bounds[MJPSInfo::downright][MGBInfo::mincol])
                    << " " << (int)(mGBInfoMap[j][i].bounds[MJPSInfo::downright][MGBInfo::maxcol]) << "\t";
    }
#endif
}

void GBOptimization::setGBInfoMap()
{
    for (int y = 0; y < sizeY; ++y)
    {
        for (int x = 0; x < sizeX; ++x)
        {
            for (int dir = 0; dir < 8; ++dir)
            {
                mGBInfoMap[y][x].bounds[dir][MGBInfo::minrow] = sizeY;
                mGBInfoMap[y][x].bounds[dir][MGBInfo::maxrow] = 0;
                mGBInfoMap[y][x].bounds[dir][MGBInfo::mincol] = sizeX;
                mGBInfoMap[y][x].bounds[dir][MGBInfo::maxcol] = 0;
            }
        }
    }
    initDijkstraMap();
    for (int y = 0; y < sizeY; ++y)
    {
        for (int x = 0; x < sizeX; ++x)
        {
            if (isblocked(x, y))
                continue;
            dijkstraFlood(x, y);
            for (int i = 0; i < sizeY; ++i)
            {
                for (int j = 0; j < sizeX; ++j)
                {
                    if (isblocked(j, i))
                        continue;
                    int iteration = mDijkstraMap[i][j].iteration;
                    int status = mDijkstraMap[i][j].listStatus;
                    int dir = mDijkstraMap[i][j].dirFromStart;
                    if (
                            iteration == mGBIteration &&
                            status == DijkstraNode::OnClosed &&
                            dir <= 7 && dir >= 0
                            )
                    {
                        int row = mDijkstraMap[i][j].row;
                        int col = mDijkstraMap[i][j].col;
                        if (mGBInfoMap[y][x].bounds[dir][MGBInfo::minrow] > row)
                            mGBInfoMap[y][x].bounds[dir][MGBInfo::minrow] = row;
                        if (mGBInfoMap[y][x].bounds[dir][MGBInfo::maxrow] < row)
                            mGBInfoMap[y][x].bounds[dir][MGBInfo::maxrow] = row;
                        if (mGBInfoMap[y][x].bounds[dir][MGBInfo::mincol] > col)
                            mGBInfoMap[y][x].bounds[dir][MGBInfo::mincol] = col;
                        if (mGBInfoMap[y][x].bounds[dir][MGBInfo::maxcol] < col)
                            mGBInfoMap[y][x].bounds[dir][MGBInfo::maxcol] = col;
                    }
                }
            }
        }
		qDebug() << y * sizeX << "is finish";
    }
    return;
}

void GBOptimization::getIdentifySuccessors(MAstarPoint * _p)
{
    mSuccessors.clear();
    for (short i = 0; i < 8; ++i)
    {
		if (!isInGB(_p, i))
			continue;
        auto jp = jump(_p->x, _p->y, i);
        if (jp)
        {
            if (!jp->isClose)
                mSuccessors.push_back(jp);
        }
    }
}


bool GBOptimization::isInGB(MPoint * _l, short _dir)
{
    unsigned row = mEnd->y;
    unsigned col = mEnd->x;
    if (
            row >= mGBInfoMap[_l->y][_l->x].bounds[_dir][MGBInfo::minrow] &&
            row <= mGBInfoMap[_l->y][_l->x].bounds[_dir][MGBInfo::maxrow] &&
            col >= mGBInfoMap[_l->y][_l->x].bounds[_dir][MGBInfo::mincol] &&
            col <= mGBInfoMap[_l->y][_l->x].bounds[_dir][MGBInfo::maxcol]
            )
        return true;
    else
        return false;
}

void GBOptimization::initDijkstraMap()
{
    mGBIteration = 1;
    mDijkstraMap.resize(sizeY);
    for (unsigned y = 0; y < sizeY; ++y)
    {
        mDijkstraMap[y].resize(sizeX);
        {
            for (unsigned x = 0; x < sizeX; ++x)
            {
                mDijkstraMap[y][x].row = y;
                mDijkstraMap[y][x].col = x;
                mDijkstraMap[y][x].iteration = 0;
                mDijkstraMap[y][x].listStatus = DijkstraNode::OnNone;
            }
        }
    }
}
void GBOptimization::dijkstraFlood(unsigned _x, unsigned _y)
{
    mGBIteration++;
    mDijkstraList.clear();
    std::make_heap(mDijkstraList.begin(), mDijkstraList.end(), cmpGivenCost());

    if (!isblocked(_x, _y))
    {
        DijkstraNode* start = &mDijkstraMap[_y][_x];
        start->parent = NULL;
        start->dirFromStart = 255;
        start->dirFromParent = 255;
        start->givenCost = 0;
        start->iteration = mGBIteration;
        start->listStatus = DijkstraNode::OnOpen;
        for (int i = 0; i < 8; ++i)
        {
            explore(start, i, true);
        }
        start->listStatus = DijkstraNode::OnClosed;
    }

    while (!mDijkstraList.empty())
    {
        DijkstraNode* node = mDijkstraList.front();
        std::pop_heap (mDijkstraList.begin(), mDijkstraList.end(), cmpGivenCost());
        mDijkstraList.pop_back();
        auto & currentPoint = mJPSInfoMap[node->row][node->col];
        int dx = mDirection[node->dirFromParent].first;
        int dy = mDirection[node->dirFromParent].second;

        if (currentPoint.distance[node->dirFromParent] != 0)
            explore(node, node->dirFromParent);
        if (!dx)// v
        {
            short dir = mPair2DirHash[std::make_pair(-1, dy)];
            if (currentPoint.distance[mPair2DirHash[std::make_pair(-1, 0)]] == 0 && currentPoint.distance[dir] != 0)
                explore(node, dir);
            dir = mPair2DirHash[std::make_pair(1, dy)];
            if (currentPoint.distance[mPair2DirHash[std::make_pair(1, 0)]] == 0 && currentPoint.distance[dir] != 0)
                explore(node, dir);
        }
        else if (!dy) // v
        {
            short dir = mPair2DirHash[std::make_pair(dx, -1)];
            if (currentPoint.distance[mPair2DirHash[std::make_pair(0, -1)]] == 0 && currentPoint.distance[dir] != 0)
                explore(node, dir);
            dir = mPair2DirHash[std::make_pair(dx, 1)];
            if (currentPoint.distance[mPair2DirHash[std::make_pair(0, 1)]] == 0 && currentPoint.distance[dir] != 0)
                explore(node, dir);
        }
        if (dx && dy)
        {
            short dir = mPair2DirHash[std::make_pair(0, dy)];
            if (currentPoint.distance[dir] != 0)
                explore(node, dir);
            dir = mPair2DirHash[std::make_pair(dx, 0)];
            if (currentPoint.distance[dir] != 0)
                explore(node, dir);
            dir = mPair2DirHash[std::make_pair(-dx, dy)];
            auto blockdir = mPair2DirHash[std::make_pair(-dx, 0)];
            if (currentPoint.distance[dir] != 0 && currentPoint.distance[blockdir] == 0)
                explore(node, dir);
            dir = mPair2DirHash[std::make_pair(dx, -dy)];
            blockdir = mPair2DirHash[std::make_pair(0, -dy)];
            if (currentPoint.distance[dir] != 0 && currentPoint.distance[blockdir] == 0)
                explore(node, dir);
        }
        node->listStatus = DijkstraNode::OnClosed;
    }
}

void GBOptimization::explore(DijkstraNode * _node, short _dir , bool isStart/* = false*/)
{
    int dx = mDirection[_dir].first;
    int dy = mDirection[_dir].second;
    int nx = _node->col + dx;
    int ny = _node->row + dy;
    if (!isValid(nx, ny) || isblocked(nx, ny))
        return;

    DijkstraNode * next = &mDijkstraMap[ny][nx];
    auto newcost = calG(_node, next);
    //auto equcost = (dx && dy) ? newcost - (dCost - hCost + 1) : newcost;
    if (next->iteration != mGBIteration)
    {
        next->parent = _node;
        if (isStart)
            next->dirFromStart = _dir;
        else
            next->dirFromStart = _node->dirFromStart;
        next->dirFromParent = _dir;
        next->givenCost = newcost;
        //next->equalCost = equcost;
        next->listStatus = DijkstraNode::OnOpen;
        next->iteration = mGBIteration;

        mDijkstraList.push_back(next);
        std::push_heap(mDijkstraList.begin(), mDijkstraList.end(), cmpGivenCost());
    }
    else if (
                next->listStatus == DijkstraNode::OnOpen &&
                newcost < next->givenCost
             )
    {
        next->parent = _node;
        next->dirFromStart = _node->dirFromStart;
        next->dirFromParent = _node->dirFromParent;
        next->givenCost = newcost;

        unsigned size = mDijkstraList.size();
        unsigned current = 0;
        for (unsigned i = 0; i < size; ++i)
        {
            if (mDijkstraList[i] == next)
            {
                current = i;
                break;
            }
        }

        unsigned father = ((current + 1) >> 1) - 1;
        if (current > 0 && mDijkstraList[father]->givenCost > mDijkstraList[current]->givenCost)
        {
            do
            {
                std::swap(mDijkstraList[current], mDijkstraList[father]);
                current = father;
                father = ((current + 1) >> 1) - 1;
            } while (current > 0 && mDijkstraList[father]->givenCost > mDijkstraList[current]->givenCost);
            return;
        }
    }
}

unsigned GBOptimization::calG(DijkstraNode * _l, DijkstraNode * _n)
{
    unsigned deltaG;
    if (_n->col == _l->col)
        deltaG = vCost * abs((int)(_n->row - _l->row));
    else if (_n->row == _l->row)
        deltaG = hCost * abs((int)(_n->col - _l->col));
    else
        deltaG = dCost * abs((int)(_n->col - _l->col));
    unsigned fatherG = _l ? _l->givenCost : 0;
    return fatherG + deltaG;
}

