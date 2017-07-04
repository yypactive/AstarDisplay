#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>

#include <QtWidgets/QDialog>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

#include "viewwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:

    ViewWidget * mViewWidget;

    QPushButton * mStartButton;
    QPushButton * mLoadButton;
    QPushButton * mEndButton;

    QLabel * mLogLabel;

    QHBoxLayout * mButtonLayout;
    QHBoxLayout * mLogLayout;
    QVBoxLayout * mMainLayout;


};

#endif // MAINWINDOW_H
