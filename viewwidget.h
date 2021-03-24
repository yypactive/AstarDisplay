#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QWidget>
#include <view.h>
//#include <QSize>
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QTimer>
#include <vector>
#include <set>
#include <algorithm>
#include <unordered_map>
#include "astaralgorithm.h"
#include "dijkstraalgorithm.h"
#include "jpsalgorithm.h"
#include "jpsplusalgorithm.h"
#include "dstarlitealgorithm.h"
#include "hpastaralgorithm.h"
#include "gboptimization.h"

struct HashQPoint
{
    std::size_t operator()(QPoint _p) const
    {
        return ((std::hash<int>()(_p.x())
                 ^ (std::hash<int>()(_p.y()) << 1)) >> 1);
    }
};

class ViewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ViewWidget(QWidget *parent = 0);
    ~ViewWidget();
    void loadMap(const Map & _map, std::string _filename);
    void runGame();
    void freeAll();
    void updateSence();

    bool getMapValid() { return mMapValid; }
    bool getRunGameValid() { return mRunGameValid; }
    void setRunGameValid(bool _v) { mRunGameValid = _v; }
    void setDisplayValid(bool _v) { mDisplayValid = _v; }
    bool getDisplayValid() { return mDisplayValid; }
    bool getTimerActive() { return mDisplayTimer->isActive(); }
    void pauseTimer();
    void continueTimer();
    bool getInfoFlag() { return mInfoValid; }
    void setInfoFlag(bool _v) { mInfoValid = _v; }

    void useDijkstraAlgorithm(const Map &_map);
    void useAstarAlgorithm(const Map &_map);
    void useHPAstarAlgorithm(const Map &_map);
    void useJPSAlgorithm(const Map &_map);
    void useJPSPlusAlgorithm(const Map &_map);
    void useJPSPlusGBAlgorithm(const Map &_map, std::string _file);
    void useDstarLiteAlgorithm(const Map &_map);

public:
    const static unsigned sizeX = 600;
    const static unsigned sizeY = 600;
    const static unsigned timeInterval = 100;

signals:
    void sendLog(QString);
    void startDisplay();
    void endDisplay();
    void changeMap(MPoint, int);

public slots:
    void setPoint(QPointF);
    void changeBlock(QPointF);
    void moveDisplay();

private:
    void initConnect();
    void freeMap();
    void freeStart();

    void setStart(const QPoint & _s);
    void setEnd(const QPoint & _pos);



private:
    View * mView;
    QGraphicsScene * mScene;

    QGraphicsRectItem * mStartItem;
    QGraphicsRectItem * mEndItem;
    std::vector < QGraphicsRectItem * > mMapItemVec;
    std::unordered_map <QPoint,  QGraphicsRectItem *, HashQPoint> mBlockItemHash;
    std::vector < QGraphicsRectItem * > mPathItemVec;
    std::vector < QGraphicsRectItem * > mInfo1ItemVec;
    std::vector < QGraphicsRectItem * > mInfo2ItemVec;

    bool mStartValid;
    bool mEndValid;
    bool mPathValid;
    bool mMapValid;
    bool mDisplayValid;
    bool mRunGameValid;
    bool mInfoValid;

    bool mPauseState;

    bool mBlockChange;
    bool mStartChange;
    bool mEndChange;
    bool mPathChange;

    unsigned numX;
    unsigned numY;
    int margrinX;
    int margrinY;
    int rectX;
    int rectY;

    std::vector < std::vector <QPointF> > mLocation;
    std::vector <QPoint> mBlockVec;
    std::vector <QPoint> mInfo1Vec;
    std::vector <QPoint> mInfo2Vec;
    std::vector <QPoint> mPathVec;
    QPoint mStart;
    QPoint mEnd;
    QPoint mMapChangePoint;
    int mIsBlock;
    PathFindingAlgorithm * mAlgorithm;

    QTimer * mDisplayTimer;

};

#endif // VIEWWIDGET_H
