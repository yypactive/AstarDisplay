#ifndef PARSEMAP_H
#define PARSEMAP_H

#include "types.h"
using namespace std;
class ParseMap
{
public:
    ParseMap();
    virtual ~ParseMap();

	virtual int readMap(const std::string & _fileName);
    Map & getMap() { return mMap; }

protected:
    Map mMap;
};

#endif // PARSEMAP_H
