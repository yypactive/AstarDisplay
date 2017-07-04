#include "mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //int winw = width();
    //int winh = height();
    qDebug() << "w = " << width();
    qDebug() << "w = " << height();
    QWidget * widget=new QWidget();
    setCentralWidget(widget);

    mViewWidget = new ViewWidget;

    mStartButton = new QPushButton("Start");
    mLoadButton = new QPushButton("Load");
    mEndButton = new QPushButton("End");
    mButtonLayout = new QHBoxLayout;
    mButtonLayout->addWidget(mStartButton);
    mButtonLayout->addWidget(mLoadButton);
    mButtonLayout->addWidget(mEndButton);

    mLogLabel = new QLabel("Welcome to the display");
    mLogLayout = new QHBoxLayout;
    mLogLayout->addWidget(mLogLabel);

    mMainLayout = new QGridLayout;
    mMainLayout->addWidget(mViewWidget, 0, 0, 8, -1, Qt::AlignCenter | Qt::AlignVCenter);
    mMainLayout->addLayout(mButtonLayout, 8, 0, 1, -1, Qt::AlignCenter | Qt::AlignVCenter);
    mMainLayout->addLayout(mLogLayout, 9, 0, 1, -1, Qt::AlignCenter | Qt::AlignVCenter);

    centralWidget()->setLayout(mMainLayout);

    setFixedSize(1280, 960);
    qDebug() << "w = " << width();
    qDebug() << "w = " << height();

    initConnect();

}

MainWindow::~MainWindow()
{

}

void MainWindow::initConnect()
{
    connect(mStartButton, SIGNAL(clicked()), this, SLOT(startGame()));
    connect(mLoadButton, SIGNAL(clicked()), this, SLOT(LoadMap()));
    connect(mEndButton, SIGNAL(clicked()), this, SLOT(endGame()));
}

void MainWindow::startGame()
{
    qDebug() << "start Game";
}

void MainWindow::LoadMap()
{
    qDebug() << "load map";

}

void MainWindow::endGame()
{
    qDebug() << "end Game";
    close();
}
