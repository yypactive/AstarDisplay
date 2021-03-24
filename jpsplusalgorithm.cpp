#include "jpsplusalgorithm.h"
#include "types.h"

JPSPlusAlgorithm::JPSPlusAlgorithm()
{
    mDirection[MJPSInfo::downright] = std::make_pair(1, 1);
    mDirection[MJPSInfo::down] = std::make_pair(0, 1);
    mDirection[MJPSInfo::downleft] = std::make_pair(-1, 1);
    mDirection[MJPSInfo::right] = std::make_pair(1, 0);
    mDirection[MJPSInfo::left] = std::make_pair(-1, 0);
    mDirection[MJPSInfo::upright] = std::make_pair(1, -1);
    mDirection[MJPSInfo::up] = std::make_pair(0, -1);
    mDirection[MJPSInfo::upleft] = std::make_pair(-1, -1);
    mChangeValid = false;
}

void JPSPlusAlgorithm::setMap(unsigned _x, unsigned _y, const std::vector <MPoint> & _blockVec)
{
   JPSAlgorithm::setMap(_x, _y, _blockVec);
   //init
   mJPSInfoMap.resize(sizeY);
   for (int i = 0; i < sizeY; ++i)
   {
       mJPSInfoMap[i].resize(sizeX);
   }
   calJumpPointMap();
   calDistantJumpPointMap();
#if 0
   qDebug() << "left: ";
   for (unsigned j = 0; j < sizeY; ++j)
       {
           QDebug deb = qDebug();
           for (unsigned i = 0; i< sizeX; ++i)
           {
               deb.nospace() << (int)(mJPSInfoMap[j][i].distance[MJPSInfo::left]) << "\t";
           }
       }
   qDebug() << "upleft: ";
   for (unsigned j = 0; j < sizeY; ++j)
       {
           QDebug deb = qDebug();
           for (unsigned i = 0; i< sizeX; ++i)
           {
               deb.nospace() << (int)(mJPSInfoMap[j][i].distance[MJPSInfo::upleft]) << "\t";
           }
       }
   qDebug() << "downleft: ";
   for (unsigned j = 0; j < sizeY; ++j)
       {
           QDebug deb = qDebug();
           for (unsigned i = 0; i< sizeX; ++i)
           {
               deb.nospace() << (int)(mJPSInfoMap[j][i].distance[MJPSInfo::downleft]) << "\t";
           }
       }
   qDebug() << "down: ";
   for (unsigned j = 0; j < sizeY; ++j)
       {
           QDebug deb = qDebug();
           for (unsigned i = 0; i< sizeX; ++i)
           {
               deb.nospace() << (int)(mJPSInfoMap[j][i].distance[MJPSInfo::down]) << "\t";
           }
       }
#endif
#if 0
   if (isValid(15, 11))
       qDebug() << "(15,11) up" << mJPSInfoMap[15][11].distance[MJPSInfo::up];
   if (isValid(15, 12))
       qDebug() << "(15,12) up" << mJPSInfoMap[15][12].distance[MJPSInfo::up];
   if (isValid(14, 13))
       qDebug() << "(14,13) upright" << mJPSInfoMap[14][13].distance[MJPSInfo::upright];
   if (isValid(13, 14))
       qDebug() << "(13,14) upright" << mJPSInfoMap[13][14].distance[MJPSInfo::upright];
  #endif
}

void JPSPlusAlgorithm::calJumpPointMap()
{
    for (unsigned y = 0; y < sizeY; ++y)
    {
        for (unsigned x = 0; x < sizeX; ++x)
        {
            for (int i = 0; i < 8; ++i)
            {
                auto pair = mDirection[i];
                short dx = pair.first;
                short dy = pair.second;
                if (isJumpPoint(x, y, dx, dy))
                    mJPSInfoMap[y][x].isJumpPoint[i] = true;
            }
//            if (isJumpPoint(x, y, 1, 1))
//                mJPSInfoMap[y][x].isJumpPoint[MJPSInfo::upright] = true;
//            if (isJumpPoint(x, y, 0, 1))
//                mJPSInfoMap[y][x].isJumpPoint[MJPSInfo::up] = true;
//            if (isJumpPoint(x, y, -1, 1))
//                mJPSInfoMap[y][x].isJumpPoint[MJPSInfo::upleft] = true;
//            if (isJumpPoint(x, y, 1, 0))
//                mJPSInfoMap[y][x].isJumpPoint[MJPSInfo::right] = true;
//            if (isJumpPoint(x, y, -1, 0))
//                mJPSInfoMap[y][x].isJumpPoint[MJPSInfo::left] = true;
//            if (isJumpPoint(x, y, 1, -1))
//                mJPSInfoMap[y][x].isJumpPoint[MJPSInfo::downright] = true;
//            if (isJumpPoint(x, y, 0, -1))
//                mJPSInfoMap[y][x].isJumpPoint[MJPSInfo::down] = true;
//            if (isJumpPoint(x, y, -1, -1))
//                mJPSInfoMap[y][x].isJumpPoint[MJPSInfo::downleft] = true;
        }
    }
#if 0
        std::vector<MPoint> mm;
        for (unsigned y = 0; y < sizeY; ++y)
        {
            for (unsigned x = 0; x < sizeX; ++x)
            {
                for (int i = 0; i < 8; ++i)
                {
                    if (mJPSInfoMap[y][x].isJumpPoint[i])
                    {
                        mm.push_back(MPoint(x, y));
                        //break;
                    }
                }
            }
        }
        for (unsigned i = 0; i < mm.size(); ++i)
        {
            qDebug() << mm[i].x << mm[i].y;
        }
#endif
}

void JPSPlusAlgorithm::calDistantJumpPointMap()
{
    for (int y = 0; y < sizeY; ++y)
    {
        // left
        int countLeft = -1;
        bool jpLast = false;
        for (int x = 0; x < sizeX; ++x)
        {
            if (isblocked(x, y))
            {
                countLeft = -1;
                jpLast = false;
                mJPSInfoMap[y][x].distance[MJPSInfo::left] = 0;
                continue;
            }
            countLeft++;
            if (jpLast)
            {
                mJPSInfoMap[y][x].distance[MJPSInfo::left] = countLeft;
            }
            else
            {
                mJPSInfoMap[y][x].distance[MJPSInfo::left] = -countLeft;
            }
            if (mJPSInfoMap[y][x].isJumpPoint[MJPSInfo::left])
            {
                countLeft = 0;
                jpLast = true;
            }
        }

        // right
        int countRight = -1;
        jpLast = false;
        for (int x = sizeX - 1; x >= 0; --x)
        {
            if (isblocked(x, y))
            {
                countRight = -1;
                jpLast = false;
                mJPSInfoMap[y][x].distance[MJPSInfo::right] = 0;
                continue;
            }
            countRight++;
            if (jpLast)
            {
                mJPSInfoMap[y][x].distance[MJPSInfo::right] = countRight;
            }
            else
            {
                mJPSInfoMap[y][x].distance[MJPSInfo::right] = -countRight;
            }
            if (mJPSInfoMap[y][x].isJumpPoint[MJPSInfo::right])
            {
                countRight = 0;
                jpLast = true;
            }
        }
    }
   //qDebug() << "left and right";
    for (int x = 0; x < sizeX; ++x)
    {
        // up
        int countUp = -1;
        bool jpLast = false;
        for (int y = 0; y < sizeY; ++y)
        {
            if (isblocked(x, y))
            {
                countUp = -1;
                jpLast = false;
                mJPSInfoMap[y][x].distance[MJPSInfo::up] = 0;
                continue;
            }
            countUp++;
            if (jpLast)
            {
                mJPSInfoMap[y][x].distance[MJPSInfo::up] = countUp;
            }
            else
            {
                mJPSInfoMap[y][x].distance[MJPSInfo::up] = -countUp;
            }
            if (mJPSInfoMap[y][x].isJumpPoint[MJPSInfo::up])
            {
                countUp = 0;
                jpLast = true;
            }
        }

        //down
        int countDown = -1;
        jpLast = false;
        for (int y = sizeY - 1; y >= 0; --y)
        {
            if (isblocked(x, y))
            {
                countDown = -1;
                jpLast = false;
                mJPSInfoMap[y][x].distance[MJPSInfo::down] = 0;
                continue;
            }
            countDown++;
            if (jpLast)
            {
                mJPSInfoMap[y][x].distance[MJPSInfo::down] = countDown;
            }
            else
            {
                mJPSInfoMap[y][x].distance[MJPSInfo::down] = -countDown;
            }
            if (mJPSInfoMap[y][x].isJumpPoint[MJPSInfo::down])
            {
                countDown = 0;
                jpLast = true;
            }
        }
    }
    //qDebug() << "up and down";
    for (int y = 0; y < sizeY; ++y)
    {
        for (int x = 0; x < sizeX; ++x)
        {
            if (!isblocked(x, y))
            {
                //upleft
                if ( y== 0 || x == 0 )
                    mJPSInfoMap[y][x].distance[MJPSInfo::upleft] = 0;
                else if (/*isblocked(x - 1, y) || isblocked(x, y - 1) || */isblocked(x - 1, y - 1))
                    mJPSInfoMap[y][x].distance[MJPSInfo::upleft] = 0;
                else if (/*!isblocked(x, y - 1) && !isblocked(x - 1, y) &&*/
                         (mJPSInfoMap[y-1][x-1].distance[MJPSInfo::up] > 0 || mJPSInfoMap[y-1][x-1].distance[MJPSInfo::left] > 0 || mJPSInfoMap[y-1][x-1].isJumpPoint[MJPSInfo::upleft])
                         )
                {
                    mJPSInfoMap[y][x].distance[MJPSInfo::upleft] = 1;
                }
                else
                {
                    int dis = mJPSInfoMap[y - 1][x - 1].distance[MJPSInfo::upleft];
                    if (dis > 0)
                    {
                        mJPSInfoMap[y][x].distance[MJPSInfo::upleft] = dis + 1;
                    }
                    else
                    {
                        mJPSInfoMap[y][x].distance[MJPSInfo::upleft] = dis - 1;
                    }
                }

                //upright
                if ( y== 0 || x == sizeX - 1 )
                    mJPSInfoMap[y][x].distance[MJPSInfo::upright] = 0;
                else if (/*isblocked(x + 1, y) || isblocked(x, y - 1) || */isblocked(x + 1, y - 1))
                    mJPSInfoMap[y][x].distance[MJPSInfo::upright] = 0;
                else if (/*!isblocked(x, y - 1) && !isblocked(x + 1, y) &&*/
                         (mJPSInfoMap[y-1][x+1].distance[MJPSInfo::up] > 0 || mJPSInfoMap[y-1][x+1].distance[MJPSInfo::right] > 0 || mJPSInfoMap[y-1][x+1].isJumpPoint[MJPSInfo::upright])
                         )
                {
                    mJPSInfoMap[y][x].distance[MJPSInfo::upright] = 1;
                }
                else
                {
                    int dis = mJPSInfoMap[y - 1][x + 1].distance[MJPSInfo::upright];
                    if (dis > 0)
                    {
                        mJPSInfoMap[y][x].distance[MJPSInfo::upright] = dis + 1;
                    }
                    else
                    {
                        mJPSInfoMap[y][x].distance[MJPSInfo::upright] = dis - 1;
                    }
                }
            }
        }
    }
    //qDebug() << "up lr";
    for (int y = sizeY - 1; y >= 0; --y)
    {
        for (int x = 0; x < sizeX; ++x)
        {
            if (!isblocked(x, y))
            {
                //downleft
                if ( y== sizeY - 1 || x == 0 )
                    mJPSInfoMap[y][x].distance[MJPSInfo::downleft] = 0;
                else if (/*isblocked(x - 1, y) || isblocked(x, y + 1) || */isblocked(x - 1, y + 1))
                    mJPSInfoMap[y][x].distance[MJPSInfo::downleft] = 0;
                else if (/*!isblocked(x, y + 1) && !isblocked(x - 1, y) &&*/
                         (mJPSInfoMap[y+1][x-1].distance[MJPSInfo::down] > 0 || mJPSInfoMap[y+1][x-1].distance[MJPSInfo::left] > 0 || mJPSInfoMap[y+1][x-1].isJumpPoint[MJPSInfo::downleft])
                         )
                {
                    mJPSInfoMap[y][x].distance[MJPSInfo::downleft] = 1;
                }
                else
                {
                    int dis = mJPSInfoMap[y + 1][x - 1].distance[MJPSInfo::downleft];
                    if (dis > 0)
                    {
                        mJPSInfoMap[y][x].distance[MJPSInfo::downleft] = dis + 1;
                    }
                    else
                    {
                        mJPSInfoMap[y][x].distance[MJPSInfo::downleft] = dis - 1;
                    }
                }

                //downright
                if ( y== sizeY - 1 || x == sizeX - 1 )
                    mJPSInfoMap[y][x].distance[MJPSInfo::downright] = 0;
                else if (/*isblocked(x + 1, y) || isblocked(x, y + 1) || */isblocked(x + 1, y + 1))
                    mJPSInfoMap[y][x].distance[MJPSInfo::downright] = 0;
                else if (/*!isblocked(x, y + 1) && !isblocked(x + 1, y) &&*/
                         (mJPSInfoMap[y+1][x+1].distance[MJPSInfo::down] > 0 || mJPSInfoMap[y+1][x+1].distance[MJPSInfo::right] > 0 || mJPSInfoMap[y+1][x+1].isJumpPoint[MJPSInfo::downright])
                         )
                {
                    mJPSInfoMap[y][x].distance[MJPSInfo::downright] = 1;
                }
                else
                {
                    int dis = mJPSInfoMap[y + 1][x + 1].distance[MJPSInfo::downright];
                    if (dis > 0)
                    {
                        mJPSInfoMap[y][x].distance[MJPSInfo::downright] = dis + 1;
                    }
                    else
                    {
                        mJPSInfoMap[y][x].distance[MJPSInfo::downright] = dis - 1;
                    }
                }
            }
        }
    }
    //qDebug() << "down lr";
}

bool JPSPlusAlgorithm::isJumpPoint(int _x, int _y, short _dx, short _dy)
{
    int nx = _x;
    int ny = _y;
    int lx = _x - _dx;
    int ly = _y - _dy;
    if (!isValid(nx, ny) || isblocked(nx, ny) || !isValid(lx, ly) || isblocked(lx, ly))
        return false;
    if (!_dx) //v
    {
        if (isValid(nx - 1, ny + _dy))
        {
            if (!isblocked(nx - 1, ny + _dy) && isblocked(nx - 1, ny))
                return true;
        }
        if (isValid(nx + 1, ny + _dy))
        {
            if (!isblocked(nx + 1, ny + _dy) && isblocked(nx + 1, ny))
                return true;
        }
        return false;
    }
    else if (!_dy) //h
    {
        if (isValid(nx + _dx, ny - 1))
        {
            if (!isblocked(nx + _dx, ny - 1) && isblocked(nx, ny - 1))
                return true;
        }
        if (isValid(nx + _dx, ny + 1))
        {
            if (!isblocked(nx + _dx, ny + 1) && isblocked(nx, ny + 1))
                return true;
        }
        return false;
    }
    else
    {
        if (isValid(lx, ny + _dy))
        {
            if (!isblocked(lx, ny + _dy) && isblocked(lx, ny))
                return true;
        }
        if (isValid(nx + _dx, ly))
        {
            if (!isblocked(nx + _dx, ly) && isblocked(nx, ly))
                return true;
        }
        return false;
    }
}

MAstarPoint * JPSPlusAlgorithm::jump(int _px, int _py, short _dir)
{
    int dx = mDirection[_dir].first * abs(mJPSInfoMap[_py][_px].distance[_dir]);
    int dy = mDirection[_dir].second * abs(mJPSInfoMap[_py][_px].distance[_dir]);
    int ex = mEnd->x - _px;
    int ey = mEnd->y - _py;
    if (dx == 0 && ex == 0 && (dy > 0 && ey > 0 && dy >= ey || dy < 0 && ey < 0 && dy <= ey) || dy == 0 && ey == 0 && (dx > 0 && ex > 0 && dx >= ex || dx < 0 && ex < 0 && dx <= ex))
    {
        return mEnd;
    }
    else if ( ( dx > 0 && ex > 0  || dx < 0 && ex < 0 ) && ( dy  > 0 && ey > 0 || dy < 0 && ey < 0 ) && (abs(ex) <= abs(dx) || abs(ey) <= abs(dy) ))
    {
        int mindis = std::min(abs(ex), abs(ey));
        int ndx = mDirection[_dir].first * mindis;
        int ndy = mDirection[_dir].second * mindis;
        return &mAstarMap[_py + ndy][_px + ndx];
    }
    else if (mJPSInfoMap[_py][_px].distance[_dir] > 0)
    {
        return &mAstarMap[_py + dy][_px + dx];
    }
    return NULL;
}

void JPSPlusAlgorithm::getIdentifySuccessors(MAstarPoint * _p)
{
    mSuccessors.clear();
    for (short i = 0; i < 8; ++i)
    {
        auto jp = jump(_p->x, _p->y, i);
        if (jp)
        {
            if (!jp->isClose)
                mSuccessors.push_back(jp);
        }
    }
}

