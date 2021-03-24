#ifndef JPSALGORITHM_H
#define JPSALGORITHM_H

#include "astaralgorithm.h"

class JPSAlgorithm : public AstarAlgorithm
{
public:
    JPSAlgorithm();
    ~JPSAlgorithm();

protected:
    virtual void getNeighbour(MAstarPoint * _p);
    virtual void getIdentifySuccessors(MAstarPoint * _p);
    virtual MAstarPoint * jump(int _px, int _py, int _dx, int _dy);
    virtual void  getNext(MAstarPoint * _p);
    void generatePath(MAstarPoint * _r);

};

#endif // JPSALGORITHM_H
