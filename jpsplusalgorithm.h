#ifndef JPSPLUSALGORITHM_H
#define JPSPLUSALGORITHM_H

#include "types.h"
#include "jpsalgorithm.h"
#include <map>
#include <iostream>


class JPSPlusAlgorithm : public JPSAlgorithm
{
public:
    JPSPlusAlgorithm();
    virtual ~JPSPlusAlgorithm() {}
    virtual void setMap(unsigned _x, unsigned _y, const std::vector <MPoint> & _blockVec);
    virtual void changePoint(const MPoint & _s, int _block) { }

protected:
    void calJumpPointMap();
    void calDistantJumpPointMap();
    bool isJumpPoint(int _x, int _y, short _dx, short _dy);
    virtual MAstarPoint * jump(int _px, int _py, short _dir);
    virtual void getIdentifySuccessors(MAstarPoint * _p);

protected:
    std::vector < std::vector <MJPSInfo> > mJPSInfoMap;
    std::map <short, std::pair <short, short> > mDirection;

};

#endif // JPSPLUSALGORITHM_H
