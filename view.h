#ifndef VIEW_H
#define VIEW_H

#include <QtCore>
#include <QGraphicsView>
#include <QtGui>
#include <string>
#include <c>
#include <QApplication>
#include "types.h"

class View : public QGraphicsView
{
    Q_OBJECT
public:
    View(QWidget *parent = 0);
    View(QGraphicsScene *scene, QWidget *parent = 0);
    ~View();
    void initView();

public slots:

signals:
   void leftPress(QPointF);
   void rightPress(QPointF);

protected:
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    void initScale();


private:
    QMatrix _one2one_matrix;
    bool mClickValid;
};

#endif // View_H
