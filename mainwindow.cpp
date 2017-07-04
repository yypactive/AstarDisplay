#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget * widget=new QWidget();
    setCentralWidget(widget);

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

    mMainLayout = new QVBoxLayout;
    mMainLayout->addLayout(mButtonLayout);
    mMainLayout->addLayout(mLogLayout);

    centralWidget()->setLayout(mMainLayout);
}

MainWindow::~MainWindow()
{

}
