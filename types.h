#ifndef TYPE_H
#define TYPE_H

#include <vector>
#include <string>
#include <limits>
#include <utility>

struct MPoint
{
    int x;
    int y;
    MPoint()
        : x(0), y(0)
    { }

    MPoint(int _x, int _y)
        : x(_x), y(_y)
    { }

    bool operator== (const MPoint& b) const
    {
        return (x == b.x) && (y == b.y);
    }
};

struct HashPoint
{
    std::size_t operator()(MPoint _p) const
    {
        return ((std::hash<int>()(_p.x)
            ^ (std::hash<int>()(_p.y) << 1)) >> 1);
    }
};

struct EqualPoint
{
    bool operator () (MPoint _a, MPoint _b) const
    {
        return _a.x  == _b.x
            && _a.y == _b.y;
    }
};

struct pairhash {
  template <typename T, typename U>
  std::size_t operator()(const std::pair<T, U> &x) const
  {
    return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
  }
};


struct Map
{
    std::string name;
    int sizeX;
    int sizeY;
    std::vector<MPoint> blocksVec;
};

struct MAstarPoint : public MPoint
{
    unsigned F;
    unsigned H;
    unsigned G;
    MAstarPoint * father;

    bool isOpen;
    bool isClose;

    MAstarPoint()
        : MPoint(),
          F(0), G(0), H(0),
          father(NULL),
          isOpen(false), isClose(false)
    { }

    MAstarPoint(int _x,int _y)
        : MPoint(_x, _y),
          F(0), G(0), H(0),
          father(NULL),
          isOpen(false), isClose(false)
    { }
    bool operator>(const MAstarPoint& b) const
    {
        return F > b.F;
    }

};

struct cmpF
{
    bool operator()(MAstarPoint * a, MAstarPoint * b)
    {
        return a->F > b->F;
    }
};

struct MDstarLitePoint : public MPoint
{
    unsigned G;
    unsigned H;
    unsigned rhs;
    std::pair < unsigned, unsigned > key;
    MDstarLitePoint * father;

    bool isOpen;

    MDstarLitePoint()
        : MPoint(),
          H(0),
          G(std::numeric_limits<unsigned>::max()), rhs(std::numeric_limits<unsigned>::max()),
          father(NULL),
          isOpen(false)
    { }

    MDstarLitePoint(int _x, int _y)
        : MPoint(_x, _y),
          H(0),
          G(std::numeric_limits<unsigned>::max()), rhs(std::numeric_limits<unsigned>::max()),
          father(NULL),
          isOpen(false)
    { }
};

struct MJPSInfo
{
public:
    enum direction {
        downright = 0,
        down,
        downleft,
        right,
        left,
        upright,
        up,
        upleft
    };

public:
    bool isJumpPoint[8];
    short distance[8];

    MJPSInfo()
    {
        for (int i = 0; i < 8; ++i)
        {
            isJumpPoint[i] = false;
            distance[i] = 0;
        }
    }
};

struct MGBInfo
{
public:
    enum GBenum
    {
        minrow = 0,
        maxrow,
        mincol,
        maxcol
    };

public:
    short bounds[8][4];
    MGBInfo()
    {
        for (int i = 0; i < 8; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                bounds[i][j] = 0;
            }
        }
    }
};

#endif // TYPE_H

