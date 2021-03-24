#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QDialog>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QFileDialog>
#include <QStatusBar>
#include <QtGui>
#include <QtCore>
#include <QString>
#include <QtWidgets>

#include <string>

#include "viewwidget.h"
#include "streamparsexml.h"
#include "parsemap.h"

#include "types.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static const unsigned initSizeX = 760;
    static const unsigned initSizeY = 900;
    static const unsigned initViewRow = 8;
    static const unsigned initButtonRow = 1;

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void initConnect();
    void loadMapFile();
    bool loadMapFile(const std::string & _fileName);

public slots:
    void startGame();
    void loadMap();
    void pauseDisplay();
    void endGame();
    void showInfo();

    void openPause();
    void closePause();

    void setDijkstraAlgorithm();
    void setAstarAlgorithm();
    void setHPAstarAlgorithm();
    void setJPSAlgorithm();
    void setJPSPlusAlgorithm();
    void setJPSPlusGBAlgorithm();
    void setDstarLiteAlgorithm();

    void refreshMap(MPoint, int);

private:

    ViewWidget * mViewWidget;

    QAction * mLoadAction;
    QAction * mEndAction;

    QAction * mStartAction;
    QAction * mPauseAction;
    QAction * mShowInfoAction;

    QAction * mDijkstraAction;
    QAction * mAstarAction;
    QAction * mHPAstarAction;
    QAction * mJPSAction;
    QAction * mJPSPlusAction;
    QAction * mJPSPlusGBAction;
    QAction * mDstarLiteAction;

    QMenu * mFileMenu;
    QMenu * mActionMenu;
    QMenu * mAlgorithmMenu;
    QToolBar * mToolBar;

    QLabel * mLogLabel;

    bool mLoadFlag;
    bool mStartFlag;
    Map mNowMap;
    StreamParseXml mXmlParser;
    ParseMap mMapParser;
    std::string mFileName;
};

#endif // MAINWINDOW_H
