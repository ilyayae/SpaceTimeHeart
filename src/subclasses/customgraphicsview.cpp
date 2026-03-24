#include "include/subclasses/customgraphicsview.h"

CustomGraphicsView::CustomGraphicsView(QWidget *parent)
    : QGraphicsView(parent)
{
    setScene(new QGraphicsScene(this));
    setDragMode(QGraphicsView::ScrollHandDrag);
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
