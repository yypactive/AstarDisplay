#include "jpsalgorithm.h"


JPSAlgorithm::JPSAlgorithm()
{
}

JPSAlgorithm::~JPSAlgorithm()
{

}

void JPSAlgorithm::getNeighbour(MAstarPoint * _p)
{
    mSuccessors.clear();
    for (int i = _p->y - 1; i < _p->y + 2; ++i)
    {
        for (int j = _p->x - 1; j < _p->x + 2; ++j)
        {
            if (isValid(j, i) && !isblocked(j, i))
                     mSuccessors.push_back(&mAstarMap[i][j]);
        }
    }
}

void JPSAlgorithm::getIdentifySuccessors(MAstarPoint * _p)
{
    /*std::vector <MAstarPoint * > neighbour;
    getNeighbour(_p);
    std::swap(neighbour, mSuccessors);
    for (int i = 0; i < neighbour.size(); ++i)
    {
        int chax = neighbour[i]->x - _p->x;
        int dx = chax >= 1 ? 1 : ( chax < -1 ? -1 : chax);
        int chay = neighbour[i]->y - _p->y;
        int dy = chay >= 1 ? 1 : ( chay < -1 ? -1 : chay);
        MAstarPoint * jumpPoint = jump(_p->x, _p->y, dx, dy);
        if (jumpPoint)
            mSuccessors.push_back(jumpPoint);
    }*/
    mSuccessors.clear();
    for (int dx = -1; dx < 2; ++dx)
    {
        for (int dy = -1; dy < 2; ++dy)
        {
            if (dx || dy)
            {
                MAstarPoint * jumpPoint = jump(_p->x, _p->y, dx, dy);
                if (jumpPoint)
                    mSuccessors.push_back(jumpPoint);
            }
        }
    }
}

MAstarPoint * JPSAlgorithm::jump(int _px, int _py, int _dx, int _dy)
{
    /*
    int nx = _px + _dx;
    int ny = _py + _dy;
    if (!isValid(nx, ny))
        return false;
    if (!isValid(&mAstarMap[_py][_px], &mAstarMap[ny][nx]))
        return false;

    if (mEnd->x == nx && mEnd->y == ny)
        return &mAstarMap[ny][nx];
    //forced h
    if (_dx != 0 && _dy == 0)
    {
        if ((nx + _dx) > -1 && (nx + _dx) < mMap[0].size())
        {
            if (ny > 0)
                if (mMap[ny - 1][nx] == 1 && mMap[ny - 1][nx +_dx] == 0)
                    return &mAstarMap[ny][nx];
            if (ny < mMap.size() - 1)
                if (mMap[ny + 1][nx] == 1 && mMap[ny + 1][nx +_dx] == 0)
                    return &mAstarMap[ny][nx];
        }
    }
    else if (_dx == 0 && _dy != 0) //forced v
    {
        if ((ny + _dy) > -1 && (ny + _dy) < mMap.size())
        {
            if (nx > 0)
                if (mMap[ny][nx - 1] == 1 && mMap[ny + _dy][nx - 1] == 0)
                    return &mAstarMap[ny][nx];
            if (nx < mMap[0].size() - 1)
                if (mMap[ny][nx + 1] == 1 && mMap[ny + _dy][nx + 1] == 0)
                    return &mAstarMap[ny][nx];
        }
    }
    else //forced d
    {
        if ((ny + _dy) > -1 && (ny + _dy) < mMap.size())
            if (mMap[ny][_px] == 1 &&  mMap[ny + _dy][_px] == 0)
                return &mAstarMap[ny][nx];
        if ((nx + _dx) > -1 && (nx + _dx) < mMap[0].size())
            if (mMap[_py][nx] == 1 && mMap[_py][nx + _dx] == 0)
                return &mAstarMap[ny][nx];
        if (jump(nx, ny, _dx, 0) != NULL || jump(nx, ny, 0, _dy) != NULL)
        {
            return &mAstarMap[ny][nx];
        }
    }
    return jump(nx, ny, _dx, _dy);
    */
    int jx = _px + _dx;
    int jy = _py + _dy;
    int lx = _px;
    int ly = _py;
    if (!isValid(lx, ly))
        return NULL;
    if (!isValid(jx, jy))
        return NULL;
    int tx;
    int ty;
    int tlx;
    int tly;
    if (_dx != 0 && _dy == 0) // h
    {
        while (isValid(&mAstarMap[ly][lx], &mAstarMap[jy][jx]))
        {
            if (mEnd->x == jx && mEnd->y == jy)
                return &mAstarMap[jy][jx];
            if ((jx + _dx) > -1 && (jx + _dx) < sizeX)
            {
                if (jy > 0)
                {
                    if (mMap[jy - 1][jx] == 1 && mMap[jy - 1][jx + _dx] == 0)
                        return &mAstarMap[jy][jx];
                }
                if (jy < sizeY - 1)
                {
                    if (mMap[jy + 1][jx] == 1 && mMap[jy + 1][jx +_dx] == 0)
                        return &mAstarMap[jy][jx];
                }
            }
            lx = jx;
            jx += _dx;
            if (!isValid(jx, jy))
                return NULL;
        }
    }
    else if (_dx == 0 && _dy != 0) //forced v
    {
        while (isValid(&mAstarMap[ly][lx], &mAstarMap[jy][jx]))
        {
            if (mEnd->x == jx && mEnd->y == jy)
                return &mAstarMap[jy][jx];
            if ((jy + _dy) > -1 && (jy + _dy) < sizeY)
            {
                if (jx > 0)
                    if (mMap[jy][jx - 1] == 1 && mMap[jy + _dy][jx - 1] == 0)
                        return &mAstarMap[jy][jx];
                if (jx < sizeX - 1)
                    if (mMap[jy][jx + 1] == 1 && mMap[jy + _dy][jx + 1] == 0)
                        return &mAstarMap[jy][jx];
            }
            ly = jy;
            jy += _dy;
            if (!isValid(jx, jy))
                return NULL;
        }

    }
    else //forced d
    {
        while (isValid(&mAstarMap[ly][lx], &mAstarMap[jy][jx]))
        {
            if (mEnd->x == jx && mEnd->y == jy)
                return &mAstarMap[jy][jx];
            if ((jy + _dy) > -1 && (jy + _dy) < sizeY)
                if (mMap[jy][lx] == 1 &&  mMap[jy + _dy][lx] == 0)
                    return &mAstarMap[jy][jx];                         ;
            if ((jx + _dx) > -1 && (jx + _dx) < sizeX)
                if (mMap[ly][jx] == 1 && mMap[ly][jx + _dx] == 0)
                    return &mAstarMap[jy][jx];
            tx = jx + _dx;
            ty = jy;
            tlx = jx;
            tly = jy;
            while (isValid(tx, ty))
            {
                if (!isValid(&mAstarMap[tly][tlx], &mAstarMap[ty][tx]))
                    break;
                if (mEnd->x == tx && mEnd->y == ty)
                    return &mAstarMap[jy][jx];
                if ((tx + _dx) > -1 && (tx + _dx) < sizeX)
                {
                    if (ty > 0)
                    {
                        if (mMap[ty - 1][tx] == 1 && mMap[ty - 1][tx +_dx] == 0)
                             return &mAstarMap[jy][jx];
                    }
                    if (ty < sizeY - 1)
                    {
                        if (mMap[ty + 1][tx] == 1 && mMap[ty + 1][tx +_dx] == 0)
                             return &mAstarMap[jy][jx];
                    }
                }
                tlx = tx;
                tx += _dx;
            }
            tx = jx;
            ty = jy + _dy;
            tlx = jx;
            tly = jy;
            while (isValid(tx, ty))
            {
                if (!isValid(&mAstarMap[tly][tlx], &mAstarMap[ty][tx]))
                    break;
                if (mEnd->x == tx && mEnd->y == ty)
                    return &mAstarMap[jy][jx];
                if ((ty + _dy) > -1 && (ty + _dy) < sizeY)
                {
                    if (tx > 0)
                        if (mMap[ty][tx - 1] == 1 && mMap[ty + _dy][tx - 1] == 0)
                            return &mAstarMap[jy][jx];
                    if (tx < sizeX - 1)
                        if (mMap[ty][tx + 1] == 1 && mMap[ty + _dy][tx + 1] == 0)
                            return &mAstarMap[jy][jx];
                }
                tly = ty;
                ty += _dy;
            }
            lx = jx;
            ly = jy;
            jx += _dx;
            jy += _dy;
            if (!isValid(jx, jy))
                return NULL;
        }
    }
    return NULL;
}

void JPSAlgorithm::getNext(MAstarPoint * _p)
{
    getIdentifySuccessors(_p);
}

void JPSAlgorithm::generatePath(MAstarPoint * _r)
{
    mStart->father = NULL;
    mPathQue.clear();
    // need to updated
    mPathQue.push_front(MPoint(_r->x, _r->y));
    if (_r->father == NULL)
        return;
#if 1
    for (auto p = _r, f = _r->father; f != NULL; p = p->father, f = f->father)
    {
        /*if (p->x != f->x && p->y != f->y && abs(p->x - f->x) + abs(p->y - f->y) == 2)
        {
            if (mMap[p->y][f->x] == 1)
                mPathQue.push_front(MPoint(p->x, f->y));
            else if (mMap[f->y][p->x] == 1)
                mPathQue.push_front(MPoint(f->x, p->y));
            mPathQue.push_front(MPoint(f->x, f->y));
        }
        else*/
        {
            int dx = (f->x - p->x) >= 1 ? 1 : ((f->x - p->x) < -1 ? -1 : (f->x - p->x));
            int dy = (f->y - p->y) >= 1 ? 1 : ((f->y - p->y) < -1 ? -1 : (f->y - p->y));
            int nx = p->x;
            int ny = p->y;
            do
            {
                nx += dx;
                ny += dy;
                mPathQue.push_front(MPoint(nx, ny));
            }while (nx != f->x || ny != f->y);
        }
    }
#elif 0
    for (auto p = _r, f = _r->father; f != NULL; p = p->father, f = f->father)
    {
        mPathQue.push_front(MPoint(f->x, f->y));
    }
#else
    int dx = 0;
    int dy = 0;
    for (auto p = _r, f = _r->father; f != NULL; p = p->father, f = f->father)
    {
        int ndx = (f->x - p->x) >= 1 ? 1 : ((f->x - p->x) < -1 ? -1 : (f->x - p->x));
        int ndy = (f->y - p->y) >= 1 ? 1 : ((f->y - p->y) < -1 ? -1 : (f->y - p->y));
        if (ndx == dx && ndy == dy)
            mPathQue.pop_front();\
        mPathQue.push_front(MPoint(f->x, f->y));
        dx = ndx;
        dy = ndy;
    }
#endif
}
