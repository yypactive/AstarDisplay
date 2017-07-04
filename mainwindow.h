#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QVBoxLayout *mMainLayout;
    QHBoxLayout *mButtonLayout;

    //button
    QPushButton *mStartGame;
    QPushButton *mLoadMap;
    QPushButton *mEndGame;
};

#endif // MAINWINDOW_H
