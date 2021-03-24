#ifndef DIJKSTRAALGORITHM_H
#define DIJKSTRAALGORITHM_H

#include "astaralgorithm.h"

class DijkstraAlgorithm : public AstarAlgorithm
{
public:
    DijkstraAlgorithm();
    virtual ~DijkstraAlgorithm() {}

    virtual unsigned calH(MAstarPoint * _p, MAstarPoint * _e);


};

#endif // DIJKSTRAALGORITHM_H
