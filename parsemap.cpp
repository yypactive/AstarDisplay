#include <sstream>
#include <fstream>
//#include <iostream>
#include <stdlib.h>
#include "parsemap.h"

ParseMap::ParseMap()
{

}
ParseMap::~ParseMap()
{

}

int ParseMap::readMap(const std::string & _fileName)
{
    mMap.blocksVec.clear();
    mMap.name = "";
    mMap.sizeX = 0;
    mMap.sizeY = 0;
    ifstream filestream(_fileName.c_str());
    if (filestream.bad())
        return -1;
    stringstream ss;
    ss << filestream.rdbuf();
    string str;
    bool type = false;
    bool height = false;
    bool width = false;
    bool map = false;
    unsigned line = 0;

    while (ss >> str)
    {
        if (str == "type")
        {
            type = true;
        }
        else if (type)
        {
            type = false;
            mMap.name = str;
        }
        else if (str == "height")
        {
            height = true;
        }
        else if (height)
        {
            height = false;
            mMap.sizeY = atoi(str.c_str());
        }
        else if (str == "width")
        {
            width = true;
        }
        else if (width)
        {
            width = false;
            mMap.sizeX = atoi(str.c_str());
        }
        else if (str == "map")
        {
            map = true;
        }
        else if (map)
        {
            for (unsigned i = 0; i < str.size(); ++i)
            {
                if (str[i] == '@' || str[i] == 'O' || str[i] == 'T' || str[i] == 'W')
                {
                    mMap.blocksVec.push_back(MPoint(i, line));
                }
            }
            line ++;
        }
    }
    filestream.close();
    if (mMap.sizeX == 0 || mMap.sizeY == 0 || mMap.name == "")
        return -2;
    return 0;
}
