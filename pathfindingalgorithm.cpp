#include "pathfindingalgorithm.h"

PathFindingAlgorithm::PathFindingAlgorithm()
    : sizeX(0),
      sizeY(0),
      mPathQue(0),
      mMapValid(false),
      mResultValid(false),
      mVacantValid(false),
      mChangeValid(true),
      mTimeRecord(0)
{

}

PathFindingAlgorithm::~PathFindingAlgorithm()
{

}

void PathFindingAlgorithm::setMap(unsigned _x, unsigned _y, const std::vector <MPoint> & _blockVec)
{
    sizeX = _x;
    sizeY = _y;
    mMap.clear();
    mMap.resize(sizeY);
    for (unsigned i = 0; i < _y; ++i)
    {
        mMap[i].resize(sizeX);
        for (unsigned j = 0; j < _x; ++j)
            mMap[i][j] = 0;
    }
    for (unsigned i = 0; i < _blockVec.size(); ++i)
        mMap[_blockVec[i].y][_blockVec[i].x] = 1;
    mMapValid = true;
}

void PathFindingAlgorithm::setFile(std::string _filename)
{
    mFileName = _filename;
}

void PathFindingAlgorithm::changePoint(const MPoint & _s, int _block)
{
    mMap[_s.y][_s.x] = _block;
}

bool PathFindingAlgorithm::isblocked(unsigned _x, unsigned _y)
{
    return mMap[_y][_x];
}

std::vector<MPoint> & PathFindingAlgorithm::getInfo1List()
{
    mInfo1Vec.clear();
    return mInfo1Vec;
}

std::vector<MPoint> & PathFindingAlgorithm::getInfo2List()
{
    mInfo2Vec.clear();
    return mInfo2Vec;
}
