#include "include/imageAnnotationObjects/customgraphicsview.h"

CustomGraphicsView::CustomGraphicsView(QWidget *parent)
    : QGraphicsView(parent)
{
    setScene(new QGraphicsScene(this));
    setDragMode(QGraphicsView::NoDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

void CustomGraphicsView::wheelEvent(QWheelEvent *event)
{
    const double factor = 1.15;
    if (event->angleDelta().y() > 0)
    {
        if (transform().m11() < 10.0)
            scale(factor, factor);
    }
    else
    {
        if (transform().m11() > 0.1)
            scale(1.0 / factor, 1.0 / factor);
    }
    event->accept();
}


void CustomGraphicsView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        isPanning = true;
        lastPanPos = event->pos();
        firstPanPos =  event->pos();
    }
    QGraphicsView::mousePressEvent(event);
}

void CustomGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if(isPanning && !panningLocked)
    {
        QPoint delta = event->pos() - lastPanPos;
        lastPanPos = event->pos();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        viewport()->update();
    }
    QGraphicsView::mouseMoveEvent(event);
}

void CustomGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        float value = 3;
        if ((event->pos() - firstPanPos).manhattanLength() <= value)
        {
            emit clicked(event->pos());
        }
        isPanning = false;
    }
    QGraphicsView::mouseReleaseEvent(event);
}
