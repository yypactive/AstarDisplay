#include "mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      mStartFlag(false),
      mLoadFlag(false)
{
    //int winw = width();
    //int winh = height();
    qDebug() << "w = " << width();
    qDebug() << "w = " << height();    

    mViewWidget = new ViewWidget;
    setCentralWidget(mViewWidget);

    mLogLabel = new QLabel(tr("Ready"));
    statusBar()->addWidget(mLogLabel);

    mLoadAction = new QAction(QIcon(""), tr("Load"), this);
    mEndAction = new QAction(QIcon(""), tr("Exit"), this);

    mStartAction = new QAction(QIcon(""), tr("Start"), this);
    mStartAction->setDisabled(true);
    mStartAction->setShortcut(Qt::Key_Return);
    mPauseAction = new QAction(QIcon(""), tr("Pause"), this);
    mPauseAction->setDisabled(true);
    mPauseAction->setShortcut(Qt::Key_Space);
    mShowInfoAction = new QAction(QIcon(""), tr("ShowInfo"), this);
    mShowInfoAction->setDisabled(true);
    mShowInfoAction->setShortcut(Qt::Key_F1);

    mDijkstraAction = new QAction(QIcon(""), tr("Dijkstra"), this);
    mAstarAction = new QAction(QIcon(""), tr("A *"), this);
    mHPAstarAction = new QAction(QIcon(""), tr("HPA*"), this);
    mJPSAction = new QAction(QIcon(""), tr("JPS"), this);
    //mJPSAction->setDisabled(true);
    mJPSPlusAction = new QAction(QIcon(""), tr("JPS+"), this);
    //mJPSPlusAction->setDisabled(true);
    mJPSPlusGBAction = new QAction(QIcon(""), tr("JPS+GB"), this);
    mDstarLiteAction = new QAction(QIcon(""), tr("D * Lite"), this);
    mDstarLiteAction->setDisabled(true);
    setWindowTitle("D* Lite Display");

    mFileMenu = new QMenu(tr("&File"), this);
    mFileMenu->addAction(mLoadAction);
    mFileMenu->addAction(mEndAction);

    mActionMenu = new QMenu(tr("&Action"), this);
    mActionMenu->addAction(mStartAction);
    mActionMenu->addAction(mPauseAction);
    mActionMenu->addAction(mShowInfoAction);

    mAlgorithmMenu = new QMenu(tr("&Algorithm"), this);
    mAlgorithmMenu->addAction(mDijkstraAction);
    mAlgorithmMenu->addAction(mAstarAction);
    mAlgorithmMenu->addAction(mHPAstarAction);
    mAlgorithmMenu->addAction(mJPSAction);
    mAlgorithmMenu->addAction(mJPSPlusAction);
    mAlgorithmMenu->addAction(mJPSPlusGBAction);
    mAlgorithmMenu->addAction(mDstarLiteAction);

    menuBar()->addMenu(mFileMenu);
    menuBar()->addMenu(mActionMenu);
    menuBar()->addMenu(mAlgorithmMenu);

    mToolBar = new QToolBar(this);
    mToolBar->addAction(mLoadAction);
    mToolBar->addAction(mStartAction);
    mToolBar->addAction(mPauseAction);
    mToolBar->addAction(mEndAction);
    addToolBar(Qt::TopToolBarArea, mToolBar);

    resize(QSize(initSizeX, initSizeY));
    qDebug() << "w = " << width();
    qDebug() << "w = " << height();

    initConnect();

}

MainWindow::~MainWindow()
{

}

void MainWindow::initConnect()
{
    connect(mStartAction, SIGNAL(triggered()), this, SLOT(startGame()));
    connect(mLoadAction, SIGNAL(triggered()), this, SLOT(loadMap()));
    connect(mPauseAction, SIGNAL(triggered()), this, SLOT(pauseDisplay()));
    connect(mShowInfoAction, SIGNAL(triggered()), this, SLOT(showInfo()));
    connect(mEndAction, SIGNAL(triggered()), this, SLOT(endGame()));
    connect(mViewWidget, SIGNAL(sendLog(QString)), mLogLabel, SLOT(setText(QString)));
    connect(mViewWidget, SIGNAL(startDisplay()), this, SLOT(openPause()));
    connect(mViewWidget, SIGNAL(endDisplay()), this, SLOT(closePause()));
    connect(mAstarAction, SIGNAL(triggered()), this, SLOT(setAstarAlgorithm()));
    connect(mDijkstraAction, SIGNAL(triggered()), this, SLOT(setDijkstraAlgorithm()));
    connect(mHPAstarAction, SIGNAL(triggered()), this,SLOT(setHPAstarAlgorithm()));
    connect(mJPSAction, SIGNAL(triggered()), this, SLOT(setJPSAlgorithm()));
    connect(mJPSPlusAction, SIGNAL(triggered()), this, SLOT(setJPSPlusAlgorithm()));
    connect(mJPSPlusGBAction, SIGNAL(triggered()), this, SLOT(setJPSPlusGBAlgorithm()));
    connect(mDstarLiteAction, SIGNAL(triggered()), this, SLOT(setDstarLiteAlgorithm()));
   //connect(mViewWidget, SIGNAL(changeMap(MPoint,int)), this, SLOT(refreshMap(MPoint, int)));
}

void MainWindow::loadMapFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Map file"),
        ".", tr("MAP (*.map)\n"
                "TMX (*.tmx)\n"
                "All (*.*)"));
    if (!fileName.isEmpty())
    {
        mLogLabel->setText("Load Map: " + fileName);
        if ( loadMapFile(fileName.toStdString()) )
        {
            mLoadFlag = true;
            mLogLabel->setText("Load Success: " + fileName);
        }
        else
        {
             mLogLabel->setText("Load Fail: " + fileName);
        }
    }
}

bool MainWindow::loadMapFile(const std::string & _fileName)
{
    QString error_msg("An error occurred during loading map file.\nDetails:\n");
    setCursor(Qt::WaitCursor);
    try
    {

        //TODO: parse the xml, clean the old map, draw new map
        std::string suffix = _fileName.substr(_fileName.length() - 4, 4);
        if (suffix == ".tmx" || suffix == ".TMX")
        {
            if (mXmlParser.readXml(_fileName))
                throw new std::out_of_range("Error when parsing xml.");
            mNowMap = mXmlParser.getMap();
            if (mNowMap.sizeX <= 0 || mNowMap.sizeY <= 0)
                throw std::out_of_range("inValid size.");
        }
        else if (suffix == ".map" || suffix == ".MAP")
        {
            if (mMapParser.readMap(_fileName))
                throw new std::out_of_range("Error when parsing xml.");
            mNowMap = mMapParser.getMap();
            if (mNowMap.sizeX <= 0 || mNowMap.sizeY <= 0)
                throw std::out_of_range("inValid size.");
        }
        else
        {
            throw std::out_of_range("Invalid format.");
        }
        for (int i = 0; i < mNowMap.blocksVec.size();++i)
        {
            if (mNowMap.blocksVec[i].x >= mNowMap.sizeX || mNowMap.blocksVec[i].y >= mNowMap.sizeY)
                throw new std::out_of_range("inValid blocks.");
        }
#if 0
            qDebug() << "name = " << QString::fromStdString(mNowMap.name);
            qDebug() << "sizeX = " << mNowMap.sizeX;
            qDebug() << "sizeY = " << mNowMap.sizeY;
            qDebug() << mNowMap.blocksVec.size();
            for (int i = 0; i < mNowMap.blocksVec.size(); ++i)
            {
                qDebug() << "x =" << mNowMap.blocksVec[i].x << "y = " << mNowMap.blocksVec[i].y;
            }
#endif
        mViewWidget->freeAll();
        mFileName = _fileName;
        mViewWidget->loadMap(mNowMap, mFileName);
    }
    catch (std::exception &e)
    {
        QMessageBox::warning(this, "Displayer",
            error_msg + QString::fromStdString(e.what()));
        unsetCursor();
        return false;
    }
    catch (...)
    {
        QMessageBox::warning(this, "Displayer",
            "Unknown error occurred.");
        unsetCursor();
        return false;
    }

    mStartAction->setEnabled(true);
    mShowInfoAction->setEnabled(true);
    mStartAction->setText("Start");
    unsetCursor();
    return true;
}

void MainWindow::startGame()
{
    //mViewWidget->setStart(QPoint(0, 0));
    if (mViewWidget->getMapValid())
    {
        mViewWidget->runGame();

        mLogLabel->setText("Start");
        if (mStartAction->text() == "Start")
            mStartAction->setText("Reset");
        mViewWidget->setDisplayValid(false);
        mPauseAction->setEnabled(true);
        mPauseAction->setText("Pause");
        mAlgorithmMenu->setEnabled(true);
    }
}

void MainWindow::loadMap()
{
    //qDebug() << "load map";
    loadMapFile();
}

void MainWindow::pauseDisplay()
{
    if (!mViewWidget->getDisplayValid())
        return;
    //qDebug() << "pauseDisplay";
    if (!mViewWidget->getTimerActive())
    {
        mViewWidget->continueTimer();
        mPauseAction->setText("Pause");
    }
    else
    {
        mViewWidget->pauseTimer();
        mPauseAction->setText("Continue");
    }
}

void MainWindow::endGame()
{
    //qDebug() << "end Game";
    close();
}

void MainWindow::openPause()
{
    mPauseAction->setEnabled(true);
    mAlgorithmMenu->setDisabled(true);
}

void MainWindow::closePause()
{
    mPauseAction->setDisabled(true);
    mAlgorithmMenu->setEnabled(true);
}

void MainWindow::showInfo()
{
    if (mViewWidget->getInfoFlag())
    {
        mViewWidget->setInfoFlag(false);
        mShowInfoAction->setText("ShowInfo");
    }
    else
    {
        mViewWidget->setInfoFlag(true);
        mShowInfoAction->setText("HideInfo");
    }
}

void MainWindow::setDijkstraAlgorithm()
{
    setCursor(Qt::WaitCursor);
    if (mViewWidget->getMapValid())
    {
        mViewWidget->freeAll();
        mViewWidget->loadMap(mNowMap, mFileName);
        mViewWidget->runGame();
    }
    mViewWidget->useDijkstraAlgorithm(mNowMap);

    unsetCursor();
    setWindowTitle("Dijkstra Display");
    auto actions = mAlgorithmMenu->actions();
    for (int i = 0; i < actions.size(); ++i)
    {
        if (actions.at(i) == mDijkstraAction)
            actions[i]->setDisabled(true);
        else
            actions[i]->setEnabled(true);
    }
    mLogLabel->setText("Set Dijkstra algorithm");
}

void MainWindow::setAstarAlgorithm()
{
    setCursor(Qt::WaitCursor);
    if (mViewWidget->getMapValid())
    {
        mViewWidget->freeAll();
        mViewWidget->loadMap(mNowMap, mFileName);
        mViewWidget->runGame();
    }
    mViewWidget->useAstarAlgorithm(mNowMap);

    unsetCursor();
    setWindowTitle("A * Display");
    auto actions = mAlgorithmMenu->actions();
    for (int i = 0; i < actions.size(); ++i)
    {
        if (actions.at(i) == mAstarAction)
            actions[i]->setDisabled(true);
        else
            actions[i]->setEnabled(true);
    }
    mLogLabel->setText("Set A* algorithm");
}

void MainWindow::setHPAstarAlgorithm()
{
    setCursor(Qt::WaitCursor);
    if (mViewWidget->getMapValid())
    {
        mViewWidget->freeAll();
        mViewWidget->loadMap(mNowMap, mFileName);
        mViewWidget->runGame();
    }
    mViewWidget->useHPAstarAlgorithm(mNowMap);

    unsetCursor();
    setWindowTitle("HPA* Display");
    auto actions = mAlgorithmMenu->actions();
    for (int i = 0; i < actions.size(); ++i)
    {
        if (actions.at(i) == mHPAstarAction)
            actions[i]->setDisabled(true);
        else
            actions[i]->setEnabled(true);
    }
    mLogLabel->setText("Set HPA* algorithm");
}

void MainWindow::setJPSAlgorithm()
{
    setCursor(Qt::WaitCursor);
    if (mViewWidget->getMapValid())
    {
        mViewWidget->freeAll();
        mViewWidget->loadMap(mNowMap, mFileName);
        mViewWidget->runGame();
    }
    mViewWidget->useJPSAlgorithm(mNowMap);
    unsetCursor();
    setWindowTitle("JPS Display");
    auto actions = mAlgorithmMenu->actions();
    for (int i = 0; i < actions.size(); ++i)
    {
        if (actions.at(i) == mJPSAction)
            actions[i]->setDisabled(true);
        else
            actions[i]->setEnabled(true);
    }
    mLogLabel->setText("Set JPS algorithm");
}

void MainWindow::setJPSPlusAlgorithm()
{
    setCursor(Qt::WaitCursor);
    if (mViewWidget->getMapValid())
    {
        mViewWidget->freeAll();
        mViewWidget->loadMap(mNowMap, mFileName);
        mViewWidget->runGame();
    }
    mViewWidget->useJPSPlusAlgorithm(mNowMap);
    unsetCursor();
    setWindowTitle("JPS+ Display");
    auto actions = mAlgorithmMenu->actions();
    for (int i = 0; i < actions.size(); ++i)
    {
        if (actions.at(i) == mJPSPlusAction)
            actions[i]->setDisabled(true);
        else
            actions[i]->setEnabled(true);
    }
    mLogLabel->setText("Set JPS+ algorithm");
}

void MainWindow::setJPSPlusGBAlgorithm()
{
    setCursor(Qt::WaitCursor);
    if (mViewWidget->getMapValid())
    {
        mViewWidget->freeAll();
        mViewWidget->loadMap(mNowMap, mFileName);
        mViewWidget->runGame();
    }
    mViewWidget->useJPSPlusGBAlgorithm(mNowMap, mFileName);
    unsetCursor();
    setWindowTitle("JPS+GB Display");
    auto actions = mAlgorithmMenu->actions();
    for (int i = 0; i < actions.size(); ++i)
    {
        if (actions.at(i) == mJPSPlusGBAction)
            actions[i]->setDisabled(true);
        else
            actions[i]->setEnabled(true);
    }
    mLogLabel->setText("Set JPS+GB algorithm");
}

void MainWindow::setDstarLiteAlgorithm()
{
    setCursor(Qt::WaitCursor);
    if (mViewWidget->getMapValid())
    {
        mViewWidget->freeAll();
        mViewWidget->loadMap(mNowMap, mFileName);
        mViewWidget->runGame();
    }
    mViewWidget->useDstarLiteAlgorithm(mNowMap);
    unsetCursor();
    setWindowTitle("D * Lite Display");
    auto actions = mAlgorithmMenu->actions();
    for (int i = 0; i < actions.size(); ++i)
    {
        if (actions.at(i) == mDstarLiteAction)
            actions[i]->setDisabled(true);
        else
            actions[i]->setEnabled(true);
    }
    mLogLabel->setText("Set D* Lite algorithm");
}

void MainWindow::refreshMap(MPoint _p, int _block)
{
    auto it = std::find(mNowMap.blocksVec.begin(), mNowMap.blocksVec.end(), _p);
    if (_block && it == mNowMap.blocksVec.end())
    {
        mNowMap.blocksVec.push_back(_p);
    }
    else if (!_block && it != mNowMap.blocksVec.end())
    {
        mNowMap.blocksVec.erase(it);
    }
}
