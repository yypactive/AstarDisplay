#include "hpastaralgorithm.h"

HPAstarAlgorithm::HPAstarAlgorithm()
    : mLevel(0),
      mEdgeSize(05)
{
}

void HPAstarAlgorithm::setMap(unsigned _x, unsigned _y, const std::vector <MPoint> & _blockVec)
{
    PathFindingAlgorithm::setMap(_x, _y, _blockVec);
    mMaxCluster.ltx = 0;
    mMaxCluster.lty = 0;
    mMaxCluster.sx = sizeX;
    mMaxCluster.sy = sizeY;

    mHPAstarInfoMap.clear();
    mAstarMap.clear();

    mHPAstarInfoMap.resize(sizeY);
    for (auto i = 0; i < sizeY; ++i)
    {
        mHPAstarInfoMap[i].reserve(sizeX);
        for (auto j = 0; j < sizeX; ++j)
            mHPAstarInfoMap[i].push_back(MHPAstarInfo(j, i, NULL, NULL, 0));
    }

    mAstarMap.resize(sizeY);
    for (unsigned i = 0; i < sizeY; ++i)
    {
        mAstarMap[i].reserve(sizeX);
        for (unsigned j = 0; j < sizeX; ++j)
            mAstarMap[i].push_back(MAstarPoint(j, i));
    }
    mIntrList.clear();
    mEdgeList.clear();
    mClustersVec.clear();

    //qDebug() << "start preprocess";
    preprocessing();
#if 0
    qDebug() << "node";
    for (unsigned j = 0; j < sizeY; ++j)
    {
        QDebug deb = qDebug();
        for (unsigned i = 0; i< sizeX; ++i)
            deb.nospace() << (int)(mHPAstarInfoMap[j][i].level) << " ";
    }
#endif
}

void HPAstarAlgorithm::preprocessing()
{
    mClustersVec.clear();
    mClustersVec.resize(mMaxlevel);
    //qDebug() << "start abstarctMaze";
    abstractMaze();
    //qDebug() << "start buildGraph";
    buildGraph();
    for (unsigned i = 2; i <= mMaxlevel; ++i)
    {
        //qDebug() << "start addLevelToGraph";
        addLevelToGraph(i);
    }
}

void HPAstarAlgorithm::abstractMaze()
{
    mIntrList.clear();
    mEdgeList.clear();
    //qDebug() << "buildCluster";
    buildCluster(1);
    mIntrList.reserve(1024);
    unsigned numx = (sizeX + mMinClusterW - 1) / mMinClusterW;
    unsigned numy = (sizeY + mMinClusterW - 1) / mMinClusterW;
    // h
    for (unsigned i = 0; i < numy; ++i)
    {
        for (unsigned j = 0; j < numx - 1; ++j)
        {
            auto &c1 = mClustersVec[0][i * numx + j];
            auto &c2 = mClustersVec[0][i * numx + j + 1];
            //qDebug() << "buildEntrance";
            buildEntrances(&mClustersVec[0][i * numx + j], &mClustersVec[0][i * numx + j + 1], 0);
        }
    }

    // v
    for (unsigned i = 0; i < numy - 1; ++i)
    {
        for (unsigned j = 0; j< numx; ++j)
        {
            auto &c1 = mClustersVec[0][i * numx + j];
            auto &c2 = mClustersVec[0][i * (numx + 1) + j];
            //qDebug() << "buildEntrance";
            buildEntrances(&mClustersVec[0][i * numx + j], &mClustersVec[0][(i + 1) * numx + j], 1);
        }
    }

}


void HPAstarAlgorithm::buildGraph()
{
    unsigned numx = (sizeX + mMinClusterW - 1) / mMinClusterW;
    for (unsigned i = 0; i < mIntrList.size(); ++i)
    {
        MHPAstarInfo * node1 = mIntrList[i].n1;
        auto x1 = node1->x;
        auto y1 = node1->y;
        node1->cluster = mIntrList[i].c1;
        node1->entrance = &mIntrList[i];

        MHPAstarInfo * node2 = mIntrList[i].n2;
        auto x2 = node2->x;
        auto y2 = node2->y;
        node2->cluster = mIntrList[i].c2;
        node2->entrance = &mIntrList[i];

        if (x1 < x2)
        {
            mIntrList[i].c1->rightnodes.push_back(node1);
            mIntrList[i].c2->leftnodes.push_back(node2);
        }
        else if (x1 > x2)
        {
            mIntrList[i].c1->leftnodes.push_back(node1);
            mIntrList[i].c2->rightnodes.push_back(node2);
        }
        else if (y1 < y2)
        {
            mIntrList[i].c1->downnodes.push_back(node1);
            mIntrList[i].c2->upnodes.push_back(node2);
        }
        else if (y1 > y2)
        {
            mIntrList[i].c1->upnodes.push_back(node1);
            mIntrList[i].c2->downnodes.push_back(node2);
        }
        if (!node1->level)
            mIntrList[i].c1->nodes.push_back(node1);
        if (!node2->level)
            mIntrList[i].c2->nodes.push_back(node2);
        node1->level = 1;
        node2->level = 1;

        //TODO: addEdge
        mEdgeList.push_back(Edge(node1, node2, 1, hCost, true));
        mIntrList[i].edge = mEdgeList.size() - 1;
        node1->edges.push_back(mEdgeList.size() - 1);
        node2->edges.push_back(mEdgeList.size() - 1);

    }
    for (unsigned i = 0; i < mClustersVec[0].size(); ++i)
    {
        for (auto it1 = mClustersVec[0][i].nodes.begin(); it1 != mClustersVec[0][i].nodes.end(); ++it1)
        {
            for (auto it2 = it1; it2 != mClustersVec[0][i].nodes.end(); ++it2)
            {
                if (it1 == it2)
                    continue;
                int d = searchforDistance(MPoint((*it1)->x, (*it1)->y), MPoint((*it2)->x, (*it2)->y), &mClustersVec[0][i]);
                if (d > 0)
                {
                    mEdgeList.push_back(Edge((*it1), (*it2), 1, d, false));
                    (*it1)->edges.push_back(mEdgeList.size() - 1);
                    (*it2)->edges.push_back(mEdgeList.size() - 1);
                }
            }
        }
    }
}

void HPAstarAlgorithm::addLevelToGraph(const unsigned _i)
{
    buildCluster(_i);
    unsigned mClusterW = mMinClusterW << (_i - 1);
    unsigned numx = (sizeX + mClusterW - 1) / mClusterW;
    unsigned numy = (sizeY + mClusterW - 1) / mClusterW;
    // h
    for (unsigned i = 0; i < numy; ++i)
    {
        for (unsigned j = 0; j < numx - 1; ++j)
        {
            Cluster &c1 = mClustersVec[_i - 1][i * numx + j];
            Cluster &c2 = mClustersVec[_i - 1][i * numx + j + 1];
            for (unsigned  y = c1.lty; y < c1.lty + c1.sy; ++y)
            {
                if (mHPAstarInfoMap[y][c2.ltx - 1].level && mHPAstarInfoMap[y][c2.ltx].level)
                {
                    c1.rightnodes.push_back(&mHPAstarInfoMap[y][c2.ltx - 1]);
                    if (mHPAstarInfoMap[y][c2.ltx - 1].level < _i)
                    {
                        c1.nodes.push_back(&mHPAstarInfoMap[y][c2.ltx - 1]);
                        mHPAstarInfoMap[y][c2.ltx - 1].level = _i;
                    }

                    c2.leftnodes.push_back(&mHPAstarInfoMap[y][c2.ltx]);
                    if (mHPAstarInfoMap[y][c2.ltx].level < _i)
                    {
                        c2.nodes.push_back(&mHPAstarInfoMap[y][c2.ltx]);
                        mHPAstarInfoMap[y][c2.ltx].level = _i;
                    }

                    if (mHPAstarInfoMap[y][c2.ltx - 1].entrance == mHPAstarInfoMap[y][c2.ltx].entrance)
                        mEdgeList[mHPAstarInfoMap[y][c2.ltx - 1].entrance->edge].level = _i;
                    else
                    {
                        bool test = false;
                        for (unsigned k = 0; k < mIntrList.size(); ++k)
                        {
                            if (mIntrList[k].n1 == &mHPAstarInfoMap[y][c2.ltx - 1] && mIntrList[k].n2 == &mHPAstarInfoMap[y][c2.ltx])
                            {
                                mEdgeList[mIntrList[k].edge].level = _i;
                                test = true;
                                break;
                            }
                        }
                        if(!test)
                        {
                            //qDebug() << "error";
                        }
                    }
                }
            }
        }
    }

    // v
    for (unsigned i = 0; i < numy - 1; ++i)
    {
        for (unsigned j = 0; j< numx; ++j)
        {
            Cluster &c1 = mClustersVec[_i - 1][i * numx + j];
            Cluster &c2 = mClustersVec[_i - 1][(i + 1) * numx + j];
            for (unsigned  x = c1.ltx; x < c1.ltx + c1.sx; ++x)
            {
                if (mHPAstarInfoMap[c2.lty - 1][x].level && mHPAstarInfoMap[c2.lty][x].level)
                {
                    c1.downnodes.push_back(&mHPAstarInfoMap[c2.lty - 1][x]);
                    if (mHPAstarInfoMap[c2.lty - 1][x].level < _i)
                    {
                        c1.nodes.push_back(&mHPAstarInfoMap[c2.lty - 1][x]);
                        mHPAstarInfoMap[c2.lty - 1][x].level = _i;
                    }

                    c2.upnodes.push_back(&mHPAstarInfoMap[c2.lty][x]);
                    if (mHPAstarInfoMap[c2.lty][x].level < _i)
                    {
                        c2.nodes.push_back(&mHPAstarInfoMap[c2.lty][x]);
                        mHPAstarInfoMap[c2.lty][x].level = _i;
                    }

                    if (mHPAstarInfoMap[c2.lty - 1][x].entrance == mHPAstarInfoMap[c2.lty][x].entrance)
                        mEdgeList[mHPAstarInfoMap[c2.lty - 1][x].entrance->edge].level = _i;
                    else
                    {
                        bool test = false;
                        for (unsigned k = 0; k < mIntrList.size(); ++k)
                        {
                            if (mIntrList[k].n1 == &mHPAstarInfoMap[c2.lty - 1][x] && mIntrList[k].n2 == &mHPAstarInfoMap[c2.lty][x])
                            {
                                mEdgeList[mIntrList[k].edge].level = _i;
                                test = true;
                                break;
                            }
                        }
                        if (!test)
                        {
                            //qDebug() << "error";
                        }
                    }
                }
            }
        }
    }

    for (unsigned i = 0; i < mClustersVec[_i - 1].size(); i++)
    {
        for (auto it1 = mClustersVec[_i - 1][i].nodes.begin(); it1 != mClustersVec[_i - 1][i].nodes.end(); ++it1)
        {
            for (auto it2 = it1; it2 != mClustersVec[_i - 1][i].nodes.end(); ++it2)
            {
                if (it1 == it2)
                    continue;
                auto node1 = (*it1);
                auto node2 = (*it2);
                int d = searchforDistance(MPoint((*it1)->x, (*it1)->y), MPoint((*it2)->x, (*it2)->y), &mClustersVec[_i - 1][i]);
                if (d > 0)
                {
                    mEdgeList.push_back(Edge((*it1), (*it2), _i, d, false));
                    (*it1)->edges.push_back(mEdgeList.size() - 1);
                    (*it2)->edges.push_back(mEdgeList.size() - 1);
                    //qDebug() << mEdgeList.size();
                }
            }
        }
    }
    //qDebug() << "add end";
}

void HPAstarAlgorithm::buildCluster(short _i)
{
    std::vector <Cluster> tmpClusters;
    unsigned mClusterW = mMinClusterW << (_i - 1);
    unsigned tmpy = sizeY;
    unsigned numx = (sizeX + mClusterW - 1) / mClusterW;
    unsigned numy = (sizeY + mClusterW - 1) / mClusterW;
    tmpClusters.reserve(numx * numy);
    for (unsigned i = 0; i < numy; i++)
    {
        unsigned height = mClusterW < tmpy ? mClusterW : tmpy;
        tmpy -= height;
        unsigned tmpx = sizeX;
        for (unsigned j = 0; j < numx; j++)
        {
            unsigned width = mClusterW < tmpx ? mClusterW : tmpx;
            tmpx -= width;
            Cluster tmpClust(j * mClusterW, i * mClusterW, width, height, _i);
            tmpClusters.push_back(tmpClust);
        }
    }
    std::swap(tmpClusters, mClustersVec[_i - 1]);
}

void HPAstarAlgorithm::buildEntrances(Cluster *_c1, Cluster *_c2, short _dir)
{
    unsigned b1;
    unsigned b2;
    unsigned bs;
    unsigned be;
/*
    if (_dir == 0) // v
    {
        b1 = _c2->ltx - 1;
        b2 = _c2->ltx;
        bs = _c2->lty;
        be = _c2->lty + _c2->sy;

        for (unsigned i = bs; i < be; ++i)
        {
            if (!isblocked(b1, i) && !isblocked(b2, i))
            {
                Entrance tmpE(&mHPAstarInfoMap[i][b1], &mHPAstarInfoMap[i][b2], _c1, _c2, true, 1);
                mIntrList.push_back(tmpE);
            }
        }
    }
    else // h
    {
        b1 = _c2->lty - 1;
        b2 = _c2->lty;
        bs = _c2->ltx;
        be = _c2->ltx + _c2->sx;

        for (unsigned i = bs; i < be; i++)
        {
            if (!isblocked(i, b1) && !isblocked(i, b2))
            {
                Entrance tmpE(&mHPAstarInfoMap[b1][i], &mHPAstarInfoMap[b2][i], _c1, _c2, true, 1);
                mIntrList.push_back(tmpE);
            }
        }
    }
*/
    if (_dir == 0) // v
    {
        b1 = _c2->ltx - 1;
        b2 = _c2->ltx;
        bs = _c2->lty;
        be = _c2->lty + _c2->sy;
        int s = bs - 1;
        for (int i = bs; i < be; ++i)
        {
            if (isblocked(b1, i) || isblocked(b2, i))
            {
                if (i > s + 1)
                {
                    unsigned size = i - s - 1;
                    if (size > 3)
                    {
                        mIntrList.push_back(Entrance(&mHPAstarInfoMap[s+1][b1], &mHPAstarInfoMap[s+1][b2], _c1, _c2, true, 1));
                        mIntrList.push_back(Entrance(&mHPAstarInfoMap[i-1][b1], &mHPAstarInfoMap[i-1][b2], _c1, _c2, true, 1));
                    }
                    else
                    {
                        mIntrList.push_back(Entrance(&mHPAstarInfoMap[(s+i)/2][b1], &mHPAstarInfoMap[(s+i)/2][b2], _c1, _c2, true, 1));
                    }
                }
                s = i;
            }
        }
        if (be > s + 1)
        {
            unsigned size = be - s - 1;
            if (size > 3)
            {
                mIntrList.push_back(Entrance(&mHPAstarInfoMap[s+1][b1], &mHPAstarInfoMap[s+1][b2], _c1, _c2, true, 1));
                mIntrList.push_back(Entrance(&mHPAstarInfoMap[be-1][b1], &mHPAstarInfoMap[be-1][b2], _c1, _c2, true, 1));
            }
            else
            {
                mIntrList.push_back(Entrance(&mHPAstarInfoMap[(s+be)/2][b1], &mHPAstarInfoMap[(s+be)/2][b2], _c1, _c2, true, 1));
            }
        }

    }
    else // h
    {
        b1 = _c2->lty - 1;
        b2 = _c2->lty;
        bs = _c2->ltx;
        be = _c2->ltx + _c2->sx;
        int s = bs - 1;
        for (int i = bs; i < be; i++)
        {
            if (isblocked(i, b1) || isblocked(i, b2))
            {
                if (i > s + 1)
                {
                    unsigned size = i - s - 1;
                    if (size > 3)
                    {
                        mIntrList.push_back(Entrance(&mHPAstarInfoMap[b1][s+1], &mHPAstarInfoMap[b2][s+1], _c1, _c2, true, 1));
                        mIntrList.push_back(Entrance(&mHPAstarInfoMap[b1][i-1], &mHPAstarInfoMap[b2][i-1], _c1, _c2, true, 1));
                    }
                    else
                    {
                        mIntrList.push_back(Entrance(&mHPAstarInfoMap[b1][(s+i)/2], &mHPAstarInfoMap[b2][(s+i)/2], _c1, _c2, true, 1));
                    }
                }
                s = i;
            }
        }
        if (be > s + 1)
        {
            unsigned size = be - s - 1;
            if (size > 3)
            {
                mIntrList.push_back(Entrance(&mHPAstarInfoMap[b1][s+1], &mHPAstarInfoMap[b2][s+1], _c1, _c2, true, 1));
                mIntrList.push_back(Entrance(&mHPAstarInfoMap[b1][be-1], &mHPAstarInfoMap[b2][be-1], _c1, _c2, true, 1));
            }
            else
            {
                mIntrList.push_back(Entrance(&mHPAstarInfoMap[b1][(s+be)/2], &mHPAstarInfoMap[b2][(s+be)/2], _c1, _c2, true, 1));
            }
        }
    }
}

int HPAstarAlgorithm::searchforDistance(const MPoint & _s, const MPoint & _e, Cluster * _c)
{
    mCurrentCluster = _c;
    mLevel = mCurrentCluster->level - 1;
    for (auto it = mCloseList.begin(); it != mCloseList.end(); it++)
    {
        (*it)->isClose = false;
    }
    for (auto it = mOpenList.begin(); it != mOpenList.end(); it++)
    {
        (*it)->isOpen = false;
    }
    mCloseList.clear();
    mOpenList.clear();

    MAstarPoint * result = AstarAlgorithm::findPath(_s, _e);
    if (result)
    {
        return result->G;
    }
    else
    {
        return -1;
    }
}

void HPAstarAlgorithm::getNext(MAstarPoint * _p)
{
    if (mLevel == 0)
    {
        AstarAlgorithm::getNext(_p);
        return;
    }
    mSuccessors.clear();
    mCostHash.clear();
    MHPAstarInfo * node = &mHPAstarInfoMap[_p->y][_p->x];
    for (auto it = node->edges.begin(); it != node->edges.end(); ++it)
    {
        auto &edge = mEdgeList[(*it)];
        if ( ( edge.isInter && edge.level >= mLevel) || ( !edge.isInter && edge.level == mLevel))
        {
            auto &newnode = (edge.s == node) ? edge.d : edge.s;
            if (isValid(newnode->x, newnode->y))
            {
                mSuccessors.push_back(&mAstarMap[newnode->y][newnode->x]);
                mCostHash[&mAstarMap[newnode->y][newnode->x]] = edge.cost;
            }
        }
    }
}

bool HPAstarAlgorithm::isValid(const int _x, const int _y)
{
    if (!mCurrentCluster->inIt(_x, _y))
        return false;
    if (mAstarMap[_y][_x].isClose)
        return false;
    else
        return true;
}

void HPAstarAlgorithm::generatePath(MAstarPoint * _r)
{
    //refinePath(mMaxlevel);
    //smoothPath();
    mPathQue.clear();
    for (auto p = _r; p != NULL; p = p->father)
    {
        mPathQue.push_front(MPoint(p->x, p->y));
    }
}

void HPAstarAlgorithm::insertNode(const MPoint & _s, short _maxl)
{
    for (unsigned l = 1; l <= _maxl; l++)
    {
        Cluster * c = NULL;
        for (unsigned i = 0; i < mClustersVec[l - 1].size(); ++i)
        {
            if (mClustersVec[l - 1][i].inIt(_s.x, _s.y))
            {
                c = &mClustersVec[l - 1][i];
                break;
            }
        }
        if (c)
        {
            connectToBorder(_s, c);
        }
    }
    //set s as max level
}

void HPAstarAlgorithm::connectToBorder(const MPoint & _s, Cluster * c)
{
    short l = c->level;
    MHPAstarInfo * node = &mHPAstarInfoMap[_s.y][_s.x];
    auto findnode = find(c->nodes.begin(), c->nodes.end(), node);
    if (findnode != c->nodes.end())// find node
        return;

    for (auto it = c->nodes.begin(); it != c->nodes.end(); ++it)
    {
        int d = searchforDistance(_s, MPoint((*it)->x, (*it)->y), c);
        if (d > 0)
        {
            mEdgeList.push_back(Edge(&mHPAstarInfoMap[_s.y][_s.x], (*it), l, d, false));
            mHPAstarInfoMap[_s.y][_s.x].edges.push_front(mEdgeList.size() - 1);
            (*it)->edges.push_front(mEdgeList.size() - 1);
            //qDebug() << "add edge: " << mEdgeList.size() - 1 << "node : (" << _s.x << "," << _s.y << ")" << "node: ("  << (*it)->x << "," << (*it)->y << ")";
        }
    }
    // test
    if (mHPAstarInfoMap[_s.y][_s.x].level < c->level)
    {
        c->nodes.push_back(&mHPAstarInfoMap[_s.y][_s.x]);
        //qDebug() << "add node: " << "mEdgeList.size() - 1" << "node : (" << _s.x << "," << _s.y << ")" << "c: ("  << c->ltx << "," << c->lty << ")";
        mDirtyClustersVec.push_back(c);
    }
}

void HPAstarAlgorithm::eraseNode(const MPoint & _s, const MPoint & _e)
{
    MHPAstarInfo &node = mHPAstarInfoMap[_s.y][_s.x];

    for (auto it = node.edges.begin(); it != node.edges.end(); ++it)
    {
        if ((*it) < mEdgeSize)
            break;
        auto &edge = mEdgeList[(*it)];
        auto target = (edge.d == &mHPAstarInfoMap[_s.y][_s.x]) ? edge.s : edge.d;
        if (target->y == _e.y && target->x == _e.x)
            continue;
        while (!target->edges.empty())
        {
            if (target->edges.front() >= mEdgeSize)
            {
                //qDebug() << "erase edge: " << target->edges.front() << "node : (" << target->x << "," << target->y << ")";
                target->edges.pop_front();
            }
            else
                break;
        }
    }
    while (!node.edges.empty())
    {
        if (node.edges.front() >= mEdgeSize)
        {
            //qDebug() << "erase edge: " << node.edges.front() << "node : (" << node.x << "," << node.y << ")";
            node.edges.pop_front();
        }
        else
            break;
    }
}

void HPAstarAlgorithm::run(const MPoint & _s, const MPoint & _e)
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

    mEdgeSize = mEdgeList.size();
    //qDebug() << "old edge size = " << mEdgeSize;
    LARGE_INTEGER t1,t2,tc;
    QueryPerformanceFrequency(&tc);
    QueryPerformanceCounter(&t1);
    //insertNode
    insertNode(_s, mMaxlevel);
    insertNode(_e, mMaxlevel);
    auto res = onlineSearch(_s, _e);
    if (res)
    {
        generatePath(res);
        smoothPath();
        smoothPath();
        mResultValid = true;
    }
    else
    {
        mResultValid = false;
    }
    QueryPerformanceCounter(&t2);
    mTimeRecord = (t2.QuadPart - t1.QuadPart) * 1.0 / tc.QuadPart;
    // eraseNode
    eraseNode(_s, _e);
    eraseNode(_e, _s);
    // erase vector;
    if (mEdgeList.size() > mEdgeSize)
        mEdgeList.erase(mEdgeList.begin() + mEdgeSize, mEdgeList.end());
    for (unsigned i = 0; i < mDirtyClustersVec.size(); ++i)
        mDirtyClustersVec[i]->nodes.pop_back();
    mDirtyClustersVec.clear();
    //qDebug() << "new edge size = " << mEdgeList.size();
    //qDebug() << "Result" << mResultValid;
    mVacantValid = true;
}

MAstarPoint * HPAstarAlgorithm::onlineSearch(const MPoint & _s, const MPoint & _e)
{
    if (_s.x == _e.x && _s.y == _e.y)
    {
        MAstarPoint * result = &mAstarMap[_e.y][_e.x];
        result->father = NULL;
        return result;
    }
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
    mAstarMap[_s.y][_s.x].father = NULL;
    mAstarMap[_e.y][_e.x].father = NULL;
    Cluster mapCluster(0, 0, sizeX, sizeY, mMaxlevel + 1);
    mCurrentCluster = &mapCluster;
    mLevel = mMaxlevel;
    MAstarPoint * result = AstarAlgorithm::findPath(_s, _e);
    if (!result)
        return result;
    unsigned count = 0;
#if 0
    for (auto p = result; p != NULL; p = p->father)
    {
        qDebug() << "(" << p->x << "," << p->y << ")";
        count ++;
        if (count > 100)
        {
            qDebug() << "error" << endl;
            break;
        }
    }
#endif
    mLevel = mMaxlevel - 1;
    while (mLevel >= 0)
    {
        unsigned mClusterW = mMinClusterW << (mLevel);
        MAstarPoint * next = result->father;
        unsigned numx = (sizeX + mClusterW - 1) / mClusterW;
        for (MAstarPoint * point = result; next != NULL; point = next)
        {
            next = point->father;
            if (!next)
                break;
            MPoint newS(point->father->x, point->father->y);
            MPoint newE(point->x, point->y);

            unsigned px = point->x / mClusterW;
            unsigned py = point->y / mClusterW;

            unsigned nx = point->father->x / mClusterW;
            unsigned ny = point->father->y / mClusterW;

            if (px == nx && py == ny)
            {
                for (auto it = mCloseList.begin(); it != mCloseList.end(); it++)
                {
                    (*it)->isClose = false;
                }
                for (auto it = mOpenList.begin(); it != mOpenList.end(); it++)
                {
                    (*it)->isOpen = false;
                }
                mCloseList.clear();
                mOpenList.clear();
                mCurrentCluster = &mClustersVec[mLevel][py * numx + px];
                AstarAlgorithm::findPath(newS, newE);
            }
        }
        mLevel--;
    }
#if 0
    for (auto p = result; p != NULL; p = p->father)
    {
        qDebug() << "(" << p->x << "," << p->y << ")";
        count ++;
        if (count > 100)
        {
            qDebug() << "error" << endl;
            break;
        }
    }
#endif
    return result;
}

void HPAstarAlgorithm::refinePath(short _maxl)
{

}

void HPAstarAlgorithm::smoothPath()
{
    if (mPathQue.size() < 3)
        return;
    std::deque < MPoint > tmpQue(0);
    std::swap(tmpQue, mPathQue);
    unsigned pathsize = tmpQue.size();
    mPathQue.push_back(tmpQue.front());
    int i = 0;
    int pdx = 0;
    int pdy = 0;
    while (i < tmpQue.size() - 1)
    {
        unsigned step = 1;
        unsigned newdx = 0;
        unsigned newdy = 0;
        for (short j = 0; j < 9; ++j)
        {
            short dx = j % 3 - 1;
            short dy = j / 3 - 1;
            if ( (!dx && !dy) || // current point
                 (dy == pdy && dx == pdx) || // prev point
                 (tmpQue[i].x + dx == tmpQue[i + 1].x && tmpQue[i].y + dy == tmpQue[i + 1].y) || // next point
                 !AstarAlgorithm::isValid(tmpQue[i].x + dx, tmpQue[i].y + dy) ) // in map
                continue;
            for (unsigned k = 1; k < pathsize - i; ++k)
            {
                if (!AstarAlgorithm::isValid(tmpQue[i].x + dx * k, tmpQue[i].y + dy * k))
                    break;
                if(isblocked(tmpQue[i].x + dx * k, tmpQue[i].y + dy * k))
                    break;
                auto it = find(tmpQue.begin() + i + 1, tmpQue.end(), MPoint(tmpQue[i].x + dx * k, tmpQue[i].y + dy * k));
                if (it != tmpQue.end())
                {
                    if (it - tmpQue.begin() - i > step)
                    {
                        step = it - tmpQue.begin() - i;
                        newdx = dx;
                        newdy = dy;
                        break;
                    }
                }
            }
        }
        if (step > 1)
        {
            unsigned nx = tmpQue[i].x + newdx;
            unsigned ny = tmpQue[i].y + newdy;
            while (!(nx == tmpQue[i + step].x && ny == tmpQue[i + step].y))
            {
                mPathQue.push_back(MPoint(nx, ny));
                nx += newdx;
                ny += newdy;
            }
        }
        mPathQue.push_back(tmpQue[i + step]);

        pdx = mPathQue[mPathQue.size() - 2].x - mPathQue.back().x;
        pdy = mPathQue[mPathQue.size() - 2].y - mPathQue.back().y;
        i += step;
    }
}

unsigned HPAstarAlgorithm::calG(MAstarPoint * _p, MAstarPoint * _l)
{
    unsigned fatherG = _p ? _p->G : 0;
    unsigned deltaG;
    if (!mLevel)
    {
        if (_p->x == _l->x)
            deltaG = vCost;
        else if (_p->y == _l->y)
            deltaG = hCost;
        else
            deltaG = dCost;
    }
    else
    {
        deltaG = mCostHash[_l];
    }
    return fatherG + deltaG;
}

std::vector<MPoint> & HPAstarAlgorithm::getInfo1List()
{
    mInfo1Vec.clear();
    return mInfo1Vec;
}

std::vector<MPoint> & HPAstarAlgorithm::getInfo2List()
{
    mInfo2Vec.clear();
    return mInfo2Vec;
}

