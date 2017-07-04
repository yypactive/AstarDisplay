#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QWidget>
#include <view.h>
#include <QGraphicsScene>

class ViewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ViewWidget(QWidget *parent = 0);

signals:

public slots:

private:
    View*				mView;
    QGraphicsScene*		mScene;

};

#endif // VIEWWIDGET_H
