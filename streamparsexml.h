#ifndef STREAMPARSEXML_H
#define STREAMPARSEXML_H

#include <QtXml/QDomDocument>
#include <QFile>
#include <QtCore/QtCore>
#include <string>
#include <QtXml/QDomElement>

#include "types.h"

class StreamParseXml
{
public:
    StreamParseXml();
    ~StreamParseXml();
    int readXml(const std::string &_fileName);
    Map & getMap();

private:
    std::vector <MPoint> string2blockVec(const std::string & srcStr);

private:
    QString mFileName;
    Map mMap;
};

#endif // STREAMPARSEXML_H
