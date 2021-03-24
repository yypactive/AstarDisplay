#ifndef STATICPFALGORITHM_H
#define STATICPFALGORITHM_H

#include "pathfindingalgorithm.h"

class StaticPFAlgorithm : public PathFindingAlgorithm
{
public:
    StaticPFAlgorithm();
    virtual ~StaticPFAlgorithm();

    virtual void rerun(const MPoint & _rs, const MPoint & _e);

};

#endif // STATICPFALGORITHM_H
