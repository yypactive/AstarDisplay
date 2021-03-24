#include "staticpfalgorithm.h"

StaticPFAlgorithm::StaticPFAlgorithm()
{

}

StaticPFAlgorithm::~StaticPFAlgorithm()
{

}

void StaticPFAlgorithm::rerun(const MPoint & _rs, const MPoint & _e)
{
    run(_rs, _e);
}
