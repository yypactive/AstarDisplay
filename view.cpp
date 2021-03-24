#include "view.h"

View::View(QWidget *parent) : QGraphicsView(parent), mClickValid(true)
{
    initView();

}

View::View(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent),
      _one2one_matrix(matrix()),
      mClickValid(true)
{
    initView();
}

View::~View()
{

}

void View::initView()
{
    qDebug() << "initView";
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setOptimizationFlag(QGraphicsView::DontSavePainterState);
    initScale();
}


void View::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (!mClickValid)
            return;
        //qDebug() << "view: press buton: " << event->pos();
        //qDebug() << "sence: press button" << mapToScene(event->pos());
        emit leftPress(mapToScene(event->pos()));

        //qDebug() << "emit";
    }
    if (event->button() == Qt::RightButton)
    {
        if (!mClickValid)
            return;
        //qDebug() << "view: press buton: " << event->pos();
        //qDebug() << "sence: press button" << mapToScene(event->pos());
        emit rightPress(mapToScene(event->pos()));

        //qDebug() << "emit";
    }
    else
    {
        QGraphicsView::mousePressEvent(event);
    }
}

void View::wheelEvent(QWheelEvent * event)
{
    if (QApplication::keyboardModifiers() == Qt::ControlModifier)
    {
        if(event->delta() > 0)
        {
            scale(1.4, 1.4);
        }
        else
        {
            scale(0.7, 0.7);
        }
    }
    else
    {
        QGraphicsView::wheelEvent(event);
    }

}

void View::initScale()
{
    QRectF scene_rect(scene()->sceneRect());
    double view_width = sizeHint().width();
    //qDebug() << scene_rect.width() << scene_rect.height();
    double view_height = sizeHint().height();
    //qDebug() << view_width << view_height;
    double full_scale = qMin(view_width / scene_rect.width(), view_height / scene_rect.height());
    //qDebug() << full_scale;
    //setMatrix(_one2one_matrix);
    //scale(full_scale, full_scale);

}

