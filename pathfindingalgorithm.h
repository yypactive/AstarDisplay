#ifndef PATHFINDINGALGORITHM_H
#define PATHFINDINGALGORITHM_H

#include "types.h"
#include <deque>
class PathFindingAlgorithm
{
public:
    PathFindingAlgorithm();
    virtual ~PathFindingAlgorithm();

    virtual void setMap(unsigned _x, unsigned _y, const std::vector <MPoint> & _blockVec);
    virtual void setFile(std::string _filename);
    virtual void changePoint(const MPoint & _s, int _block);
    virtual bool isblocked(unsigned _x, unsigned _y);
    virtual void run(const MPoint & _s, const MPoint & _e) = 0;
    virtual void rerun(const MPoint & _rs, const MPoint & _e) = 0;
    std::deque <MPoint> & getPathList() { return mPathQue; }
    virtual std::vector<MPoint> & getInfo1List();
    virtual std::vector<MPoint> & getInfo2List();
    bool getVacantValid() { return mVacantValid; }
    bool getResultValid() { return mResultValid; }
    double getRuntime() { return mTimeRecord; }
    bool getChangeValid() { return mChangeValid; }

private:


protected:
    unsigned sizeX;
    unsigned sizeY;
    std::vector < std::vector <int> > mMap;
    std::deque < MPoint > mPathQue;
    std::vector < MPoint > mInfo1Vec;
    std::vector < MPoint > mInfo2Vec;

    std::string mFileName;

    bool mMapValid;
    bool mResultValid;
    bool mVacantValid;
    double mTimeRecord;
    bool mChangeValid;
};

#endif // PATHFINDINGALGORITHM_H
