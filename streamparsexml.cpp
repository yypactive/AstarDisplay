#include "streamparsexml.h"

StreamParseXml::StreamParseXml()
{

}

StreamParseXml::~StreamParseXml()
{

}

int StreamParseXml::readXml(const std::string & _fileName)
{
    mFileName = QString::fromStdString(_fileName);
    if(mFileName.isEmpty()) return -3;
    QFile *file = new QFile(mFileName);
    if(!file->open(QFile::ReadOnly | QFile::Text)) return -2;
    QDomDocument doc;
    if (!doc.setContent(file)) { file->close(); return -1; }
    file->close();

    QDomElement docElem = doc.documentElement();
    if (docElem.tagName() != "map")
        return -4;
    Map tmpMap;
    tmpMap.sizeX = docElem.attribute("width").toInt();
    //qDebug() << "sizex = " << tmpMap.sizeX;
    tmpMap.sizeY = docElem.attribute("height").toInt();
    //qDebug() << "sizey = " << tmpMap.sizeY;
    tmpMap.name = _fileName;
    QDomNodeList layers = docElem.childNodes();
    //qDebug() << "layers:" << layers.count();
    for (int k = 0; k < layers.count(); ++k)
    {
        if (layers.at(k).isElement())
        {
            QDomElement e = layers.at(k).toElement();
            //qDebug() << qPrintable(e.tagName());
            if (e.tagName() != "layer")
                continue;
            QDomNodeList dataList = e.childNodes();
            for(int i = 0; i < dataList.count() && i < 1; ++i)
            {
                QDomNode data = dataList.at(i);
                if(!data.isElement())
                    continue;
                //qDebug() << qPrintable(data.toElement().tagName())<<qPrintable(data.toElement().text());
                if (data.toElement().tagName() != "data")
                    continue;
                std::string source = data.toElement().text().toStdString();
                tmpMap.blocksVec = string2blockVec(source);
#if 0
                for (int i = 0; i< tmpMap.blocksVec.size(); ++i)
                {
                    qDebug() << '(' << tmpMap.blocksVec[i].x << ',' << tmpMap.blocksVec[i].y << ')';
                }
#endif
            }

        }
    }
    std::swap(mMap, tmpMap);
    return 0;
}

Map & StreamParseXml::getMap()
{
    return mMap;
}

std::vector <MPoint> StreamParseXml::string2blockVec(const std::string & srcStr)
{
    unsigned nowx = 0;
    int nowy = -1;
    unsigned integer = 0;
    std::vector <MPoint> blocks;
    for (int i = 0; i < srcStr.size(); ++i)
    {
        if (srcStr[i] == ',')
        {
            if (integer)
            {
                blocks.push_back(MPoint(nowx, nowy));
            }
            nowx ++;
            integer = 0;
        }
        else if (srcStr[i] == '\n' && i > 0)
        {
            nowx = 0;
            nowy ++;
        }
        else if (srcStr[i] >= '0' && srcStr[i] <= '9')
        {
            integer *= 10;
            integer += srcStr[i] - '0';
        }

    }
    return blocks;
}
