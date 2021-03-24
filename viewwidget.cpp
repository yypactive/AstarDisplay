#include "viewwidget.h"

ViewWidget::ViewWidget(QWidget *parent)
    : QWidget(parent),
      mStartValid(false),
      mMapValid(false),
      mDisplayValid(false),
      mEndValid(false),
      mPathValid(false),
      mRunGameValid(false),
      mPauseState(false),
      mBlockChange(false),
      mStartChange(false),
      mEndChange(false),
      mPathChange(false),
      mStartItem(NULL),
      mEndItem(NULL),
      mMapItemVec(0),
      mPathItemVec(0),
      mInfoValid(false)
{
    //mAlgorithm = new AstarAlgorithm();
    //mAlgorithm = new JPSAlgorithm();
    mAlgorithm = new DstarLiteAlgorithm();
    mScene = new QGraphicsScene(this);
    mScene->setBackgroundBrush( QColor(150, 150, 150) ); // dark gray

    mScene->setSceneRect(0, 0, sizeX, sizeY);
    mView  = new View(mScene, this);
    QVBoxLayout* layout= new QVBoxLayout; // memory deallocated automatically
    layout->addWidget(mView);
    setLayout(layout);
    setAcceptDrops(true);

    //setFixedSize(700, 700);

    mDisplayTimer = new QTimer(this);
    mDisplayTimer->setInterval(timeInterval);
    mDisplayTimer->setSingleShot(false);
    emit endDisplay();
    initConnect();
}

ViewWidget::~ViewWidget()
{
    if (mAlgorithm)
        delete mAlgorithm;
}

void ViewWidget::freeAll()
{
    freeStart();
    freeMap();
    mScene->clear();
}

void ViewWidget::initConnect()
{
    connect(mView, SIGNAL(leftPress(QPointF)), this, SLOT(setPoint(QPointF)));
    connect(mView, SIGNAL(rightPress(QPointF)), this, SLOT(changeBlock(QPointF)));
    connect(mDisplayTimer, SIGNAL(timeout()), this, SLOT(moveDisplay()));
    //connect(mDisplayTimer, SIGNAL(timeout()), this, SLOT(test()));
}

void ViewWidget::freeStart()
{
    if (mStartValid)
        mStart = QPoint(0, 0);
    if (mEndValid)
        mEnd = QPoint(0, 0);
    if (mPathValid)
        mPathVec.clear();
    mStartValid = false;
    mPathValid = false;
    mEndValid = false;
    mDisplayValid = false;
    mPauseState = false;
    mStartChange = false;
    mEndChange = false;
    mPathChange = false;
    updateSence();
    mStartItem = NULL;
    mEndItem = NULL;
    mPathItemVec.clear();
}

void ViewWidget::freeMap()
{
    if (mMapValid)
    {
        mBlockVec.clear();
        mLocation.clear();
    }
    mMapValid = false;
    mMapItemVec.clear();
    mBlockItemHash.clear();
    updateSence();
}

void ViewWidget::loadMap(const Map & _map, std::string _filename)
{
    numX = _map.sizeX;
    numY = _map.sizeY;
    /*
    double width = mScene->width();
    double height = mScene->height();
    qDebug() << "loadmap: " << width << height;

    margrinX = 5;
    margrinY = 5;
    rectX = (width - 2 * margrinX) / numX;
    rectY = (height - 2 * margrinY) / numY;
    margrinX = (width - numX * rectX) / 2;
    margrinY = (height - numY * rectY) / 2;
    qDebug() << "rectX = " << rectX << "rectY = " << rectY;
    */
    margrinX = 5;
    margrinY = 5;
    rectX = 20;
    rectY = 20;
    int tmpX = 2 * margrinX + rectX * numX;
    int tmpY = 2 * margrinY + rectY * numY;
    tmpX = tmpX > mScene->width() ? tmpX : mScene->width();
    tmpY = tmpY > mScene->height() ? tmpY : mScene->height();

    margrinX = (tmpX - rectX * numX) / 2;
    margrinY = (tmpY - rectY * numY) / 2;

    mScene->setSceneRect(0, 0, tmpX, tmpY);

    mBlockVec.clear();
    for (unsigned i = 0; i < _map.blocksVec.size(); ++i)
    {
        mBlockVec.push_back(QPoint(_map.blocksVec[i].x, _map.blocksVec[i].y));
    }
    mLocation.resize(numY);
    for (unsigned i = 0; i < numY; ++i)
    {
        mLocation[i].resize(numX);
        for (unsigned j = 0; j < numX; ++j)
        {
            mLocation[i][j] = QPointF(margrinX + (j) * rectX, margrinY + (i) * rectY);
        }

    }
#if 0
    for (unsigned i = 0; i < numY; ++i)
    {
        for (unsigned j = 0; j < numX; ++j)
        {
            qDebug() << mLocation[i][j];
        }
    }
#endif
    mMapValid = true;
    updateSence();

    mAlgorithm->setFile(_filename);
    mAlgorithm->setMap(_map.sizeX, _map.sizeY, _map.blocksVec);
    mRunGameValid = false;
}

void ViewWidget::runGame()
{

    freeStart();
    mRunGameValid = true;
}

void ViewWidget::updateSence()
{
    if (mMapValid && mMapItemVec.empty() && mBlockItemHash.empty())
    {
        //update map
        for (unsigned i = 0; i < numY; ++i)
            for (unsigned j = 0; j < numX; ++j)
                mMapItemVec.push_back(mScene->addRect(QRectF(mLocation[i][j], QSizeF(rectX, rectY)), QPen(Qt::yellow)));

        //update blocks
        for (unsigned i = 0; i < mBlockVec.size(); ++i)
            mBlockItemHash[mBlockVec[i]] = mScene->addRect(QRectF(mLocation[mBlockVec[i].y()][mBlockVec[i].x()], QSizeF(rectX, rectY)), QPen(Qt::yellow), QBrush(Qt::blue));
    }

    if (mMapValid && mBlockChange)
    {
        auto it = mBlockItemHash.find(mMapChangePoint);
        if (mIsBlock && it == mBlockItemHash.end())
        {
            mBlockItemHash[mMapChangePoint] = mScene->addRect(QRectF(mLocation[mMapChangePoint.y()][mMapChangePoint.x()], QSizeF(rectX, rectY)), QPen(Qt::yellow), QBrush(Qt::blue));

        }
        else if(!mIsBlock && it != mBlockItemHash.end())
        {
            mScene->removeItem(it->second);
            mBlockItemHash.erase(it);
        }
        mBlockChange = false;
    }

    //update info
    if (!mMapValid || !mPathValid)
    {
        if (!mInfo1ItemVec.empty())
        {
            for (unsigned i = 0; i < mInfo1ItemVec.size(); ++i)
            {
                mScene->removeItem(mInfo1ItemVec[i]);
            }
            mInfo1ItemVec.clear();
            mInfo1Vec.clear();
        }
        if (!mInfo2ItemVec.empty())
        {
            for (unsigned i = 0; i < mInfo2ItemVec.size(); ++i)
            {
                mScene->removeItem(mInfo2ItemVec[i]);
            }
            mInfo2ItemVec.clear();
            mInfo2Vec.clear();
        }
    }
    if (mMapValid && mPathValid && mInfoValid)
    {
        if (mInfo1ItemVec.empty())
        {
            for (unsigned i = 0; i < mInfo1Vec.size(); ++i)
                mInfo1ItemVec.push_back(mScene->addRect(QRectF(mLocation[mInfo1Vec[i].y()][mInfo1Vec[i].x()], QSizeF(rectX, rectY)), QPen(Qt::yellow), QBrush(QColor (255, 120 , 50, 170))));
        }
        if (mInfo2ItemVec.empty())
        {
            for (unsigned i = 0; i < mInfo2Vec.size(); ++i)
                mInfo2ItemVec.push_back(mScene->addRect(QRectF(mLocation[mInfo2Vec[i].y()][mInfo2Vec[i].x()], QSizeF(rectX, rectY)), QPen(Qt::yellow), QBrush(QColor (0, 255 , 0, 170))));
        }

    }
#if 1
    //update path
    if (!mMapValid || !mPathValid)
    {
        if (!mPathItemVec.empty())
        {
            for (unsigned i = 0; i < mPathItemVec.size(); ++i)
            {
                mScene->removeItem(mPathItemVec[i]);
            }
            mPathItemVec.clear();
        }
    }
    if (mMapValid && mPathValid)
    {
        if (mPathItemVec.empty())
        {
            for (unsigned i = 0; i < mPathVec.size(); ++i)
                mPathItemVec.push_back(mScene->addRect(QRectF(mLocation[mPathVec[i].y()][mPathVec[i].x()], QSizeF(rectX, rectY)), QPen(Qt::yellow), QBrush(Qt::yellow)));
        }
        else if (mPathChange)
        {
            for (unsigned i = 0; i < mPathItemVec.size(); ++i)
            {
                mScene->removeItem(mPathItemVec[i]);
            }
            mPathItemVec.clear();
            for (unsigned i = 0; i < mPathVec.size() ; ++i)
                mPathItemVec.push_back(mScene->addRect(QRectF(mLocation[mPathVec[i].y()][mPathVec[i].x()], QSizeF(rectX, rectY)), QPen(Qt::yellow), QBrush(Qt::yellow)));
            //mStartChange = true;
            //mEndChange = true;
            mPathChange = false;
        }
    }
#endif
    //update end
    if (!mMapValid || !mEndValid)
    {
        if (mStartItem)
            mScene->removeItem(mEndItem);
    }
    if (mMapValid && mEndValid)
    {
        if (!mEndItem)
        {
            mEndItem = mScene->addRect(QRectF(mLocation[mEnd.y()][mEnd.x()], QSizeF(rectX, rectY)), QPen(Qt::yellow), QBrush(Qt::cyan));
            mEndItem->setZValue(5);
        }
        else if (mEndChange)
        {
            mScene->removeItem(mEndItem);
            mEndItem = mScene->addRect(QRectF(mLocation[mEnd.y()][mEnd.x()], QSizeF(rectX, rectY)), QPen(Qt::yellow), QBrush(Qt::cyan));
            mEndItem->setZValue(5);
            mEndChange = false;
            //mStartChange = true;
        }
    }

    //update start
    if (!mMapValid || !mStartValid)
    {
        if (mStartItem)
            mScene->removeItem(mStartItem);
    }
    if (mMapValid && mStartValid)
    {
        if (!mStartItem)
        {
            mStartItem = mScene->addRect(QRectF(mLocation[mStart.y()][mStart.x()], QSizeF(rectX, rectY)), QPen(Qt::yellow), QBrush(Qt::red));
            mStartItem->setZValue(10);
        }
        else if (mStartChange)
        {
            //mScene->
            mScene->removeItem(mStartItem);
            mStartItem = mScene->addRect(QRectF(mLocation[mStart.y()][mStart.x()], QSizeF(rectX, rectY)), QPen(Qt::yellow), QBrush(Qt::red));
            mStartItem->setZValue(10);
            mStartChange = false;
        }
    }
    mScene->update();

}

void ViewWidget::pauseTimer()
{
    mDisplayTimer->stop();
    mPauseState = true;
}

void ViewWidget::continueTimer()
{
    mDisplayTimer->start(timeInterval);
    mPauseState = false;
}

void ViewWidget::setPoint(QPointF _pos)
{
    //qDebug() << "get signal: "<< _pos;
    if ( mRunGameValid == false)
        return;
     //qDebug() << "get signal: "<< _pos;
    //if ( mDisplayValid )
        //return;
    //qDebug() << "get signal: "<< _pos;
    if (_pos.x() < margrinX || _pos.y() < margrinY)
        return;
    QPoint sencePos = QPoint((_pos.x() - margrinX) / rectX, (_pos.y() - margrinY) / rectY);
    //qDebug() << "sencePos:" << sencePos;
    if (sencePos.x() > numX  - 1|| sencePos.y() > numY - 1)
        return;
    if (!mStartValid && !mDisplayValid)
    {
        setStart(sencePos);
        qDebug() << "start";
        mStartValid = true;
    }
    else
    {
        setEnd(sencePos);
        qDebug() << "end";
    }
}

void ViewWidget::changeBlock(QPointF _pos)
{
    if ( mRunGameValid == false)
        return;
    if (_pos.x() < margrinX || _pos.y() < margrinY)
        return;
    QPoint sencePos = QPoint((_pos.x() - margrinX) / rectX, (_pos.y() - margrinY) / rectY);
    //qDebug() << "sencePos:" << sencePos;
    if (sencePos.x() > numX  - 1|| sencePos.y() > numY - 1)
        return;
    if (sencePos == mStart || sencePos == mEnd)
    {
        emit sendLog("Invalid Position for block");
        return;
    }
    if (!mAlgorithm->getChangeValid())
    {
        emit sendLog("Changing Map is not allowed in This Algorithm");
        return;
    }
    qDebug() << sencePos;
    auto it = std::find(mBlockVec.begin(), mBlockVec.end(), sencePos);
    if (it == mBlockVec.end())
    {
        qDebug() << "push back" << sencePos;
        mBlockVec.push_back(sencePos);
        mMapChangePoint = sencePos;
        mIsBlock = 1;
        mAlgorithm->changePoint(MPoint(sencePos.x(), sencePos.y()), 1);
        emit changeMap(MPoint(sencePos.x(), sencePos.y()), 1);
    }
    else
    {
        qDebug() << "pop back" << sencePos;
        mBlockVec.erase(it);
        mMapChangePoint = sencePos;
        mIsBlock = 0;
        mAlgorithm->changePoint(MPoint(sencePos.x(), sencePos.y()), 0);
        emit changeMap(MPoint(sencePos.x(), sencePos.y()), 0);
    }
    mBlockChange = true;
    updateSence();
    //mDisplayTimer->stop();
    //mDisplayValid = false;
    //emit endDisplay();
    if (!mDisplayValid)
        return;
    setCursor(Qt::WaitCursor);
    mAlgorithm->rerun(MPoint(mStart.x(), mStart.y()), MPoint(mEnd.x(), mEnd.y()));
    unsetCursor();
    QString timeStr = QString("%1 ms").arg(mAlgorithm->getRuntime() * 1000);
    mPathVec.clear();
    if (mAlgorithm->getResultValid())
    {
        emit sendLog("Re-Get Valid Path: Cost " + timeStr);
        std::deque <MPoint> pathQue = mAlgorithm->getPathList();
        mPathValid = true;
        for (unsigned i = 0; i < pathQue.size(); ++i)
        {
            mPathVec.push_back(QPoint(pathQue[i].x, pathQue[i].y));
        }
        mPathChange = true;
        updateSence();
        //mDisplayValid = true;
        //emit startDisplay();
        //mDisplayTimer->start(timeInterval);
    }
    else
    {
        mPathVec.clear();
        mPathChange = true;
        updateSence();
        emit sendLog("No Valid Path: Cost "+ timeStr);
        mPathValid = false;
    }

}

void ViewWidget::setStart(const QPoint & _s)
{
    for (int i = 0; i < mBlockVec.size(); ++i)
    {
        if (_s == mBlockVec[i])
            return;
    }
    mStart = _s;
    mStartValid = true;
    sendLog(QString("Start (%1, %2)").arg(mStart.x()).arg(mStart.y()));
    mStartChange = true;
    updateSence();
}

void ViewWidget::setEnd(const QPoint &  _pos)
{
    //if (!mStartValid)
        //return;
    sendLog(QString("End (%1, %2)").arg(mEnd.x()).arg(mEnd.y()));
    if (mStart == _pos)
        return;
    for (int i = 0; i < mBlockVec.size(); ++i)
    {
        if (_pos == mBlockVec[i])
            return;
    }
    mEnd = QPoint(_pos.x(), _pos.y());
    mEndValid = true;
    mEndChange = true;
    bool ispause = !mDisplayTimer->isActive();
    if (mDisplayValid)
    {
        mDisplayTimer->stop();
        mPathVec.clear();
        mPathChange = true;
        mDisplayValid = false;
        emit endDisplay();
    }
    mPathVec.clear();
    mPathValid = false;
    updateSence();
    setCursor(Qt::WaitCursor);
    mAlgorithm->run(MPoint(mStart.x(), mStart.y()), MPoint(mEnd.x(), mEnd.y()));
    unsetCursor();
    QString timeStr = QString("%1 ms").arg(mAlgorithm->getRuntime() * 1000);
    if (mAlgorithm->getResultValid())
    {
        emit sendLog("Get Valid Path: Cost " + timeStr);
        std::deque <MPoint> pathQue = mAlgorithm->getPathList();
        mPathValid = true;
        for (unsigned i = 0; i < pathQue.size(); ++i)
        {
            //qDebug() << pathQue[i].x <<  " " << pathQue[i].y;
            mPathVec.push_back(QPoint(pathQue[i].x, pathQue[i].y));
        }
        //display
        mInfo1Vec.clear();
        std::vector<MPoint> info1 = mAlgorithm->getInfo1List();
        qDebug() << "info1 size:" << info1.size();
        for (unsigned i = 0; i < info1.size(); ++i)
        {
            mInfo1Vec.push_back(QPoint(info1[i].x, info1[i].y));
        }
        mInfo2Vec.clear();
        std::vector<MPoint> info2 = mAlgorithm->getInfo2List();
        qDebug() << "info2 size:" << info2.size();
        for (unsigned i = 0; i < info2.size(); ++i)
        {
            mInfo2Vec.push_back(QPoint(info2[i].x, info2[i].y));
        }
        mPathChange = true;
        mEndChange = true;
        updateSence();
        qDebug() << "Display Valid: " << mDisplayValid;
        mDisplayValid = true;
        emit startDisplay();
        if (!mPauseState)
        {  
            mDisplayTimer->start();
        }

    }
    else
    {
        emit sendLog("No Valid Path: Cost "+ timeStr);
    }
}

void ViewWidget::useDijkstraAlgorithm(const Map &_map)
{
    if (mAlgorithm)
        delete mAlgorithm;
    mAlgorithm = new DijkstraAlgorithm();
    if (mMapValid)
        mAlgorithm->setMap(_map.sizeX, _map.sizeY, _map.blocksVec);
}

void ViewWidget::useAstarAlgorithm(const Map &_map)
{
    if (mAlgorithm)
        delete mAlgorithm;
    mAlgorithm = new AstarAlgorithm();
    if (mMapValid)
        mAlgorithm->setMap(_map.sizeX, _map.sizeY, _map.blocksVec);

}

void ViewWidget::useHPAstarAlgorithm(const Map &_map)
{
    if (mAlgorithm)
        delete mAlgorithm;
    mAlgorithm = new HPAstarAlgorithm();
    if (mMapValid)
        mAlgorithm->setMap(_map.sizeX, _map.sizeY, _map.blocksVec);

}

void ViewWidget::useJPSAlgorithm(const Map &_map)
{
    if (mAlgorithm)
        delete mAlgorithm;
    mAlgorithm = new JPSAlgorithm();
    if (mMapValid)
        mAlgorithm->setMap(_map.sizeX, _map.sizeY, _map.blocksVec);
}

void ViewWidget::useJPSPlusAlgorithm(const Map &_map)
{
    if (mAlgorithm)
        delete mAlgorithm;
    mAlgorithm = new JPSPlusAlgorithm();
    if (mMapValid)
        mAlgorithm->setMap(_map.sizeX, _map.sizeY, _map.blocksVec);
}

void ViewWidget::useJPSPlusGBAlgorithm(const Map &_map, std::string _file)
{
    if (mAlgorithm)
    {
        delete mAlgorithm;
    }
    mAlgorithm = new GBOptimization();
    if (mMapValid)
    {
        ((GBOptimization *)mAlgorithm)->setFile(_file);
        mAlgorithm->setMap(_map.sizeX, _map.sizeY, _map.blocksVec);
    }
}

void ViewWidget::useDstarLiteAlgorithm(const Map &_map)
{
    if (mAlgorithm)
        delete mAlgorithm;
    mAlgorithm = new DstarLiteAlgorithm();
    if (mMapValid)
        mAlgorithm->setMap(_map.sizeX, _map.sizeY, _map.blocksVec);
}

void ViewWidget::moveDisplay()
{
    if (!mPathVec.size())
    {
        mDisplayTimer->stop();
        mDisplayValid = false;
        mPathValid = false;
        updateSence();
        emit endDisplay();
        return;
    }
    mStart = mPathVec.front();
    mPathVec.erase(mPathVec.begin());
    //qDebug() << mStart;
    mPathChange = true;
    mStartChange = true;
    updateSence();
}
