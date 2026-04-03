#include "include/imageAnnotationObjects/pointhandle.h"
#include "include/imageAnnotationObjects/shapegraphicsobject.h"

PointHandle::PointHandle(ShapeGraphicsObject *parent, int id)
    : Parent(parent), Id(id)
{
    double x = Parent->MyData->XYPoints[Id].first * Parent->Image->pixmap().width();
    double y = Parent->MyData->XYPoints[Id].second * Parent->Image->pixmap().height();
    setPos(x, y);
    setRect(-SIZE / 2.0, -SIZE / 2.0, SIZE, SIZE);
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIgnoresTransformations);
    setAcceptHoverEvents(true);
}

QVariant PointHandle::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange) {
        QPointF newPos = value.toPointF();
        double imgW = Parent->Image->pixmap().width();
        double imgH = Parent->Image->pixmap().height();
        double cx = rect().center().x();
        double cy = rect().center().y();
        double minX = -cx;
        double minY = -cy;
        double maxX = imgW - cx;
        double maxY = imgH - cy;
        newPos.setX(qBound(minX, newPos.x(), maxX));
        newPos.setY(qBound(minY, newPos.y(), maxY));
        return newPos;
    }
    Parent->update();
    return QGraphicsItem::itemChange(change, value);
}

QPair<double, double> PointHandle::GetPosition()
{
    QPointF point = mapToScene(rect().center());
    return QPair<double, double>(
        (point.x()) / Parent->Image->pixmap().width(),
        (point.y()) / Parent->Image->pixmap().height()
        );
}

void PointHandle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    storedPosition = GetPosition();
    Parent->onPointClicked(this, true);
    QGraphicsEllipseItem::mousePressEvent(event);
}
void PointHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(storedPosition != GetPosition())
    {
        Parent->onPointMoved(this, storedPosition);
    }
    Parent->onPointClicked(this, false);
    QGraphicsEllipseItem::mouseReleaseEvent(event);
}

void PointHandle::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    isHovered = true;
    update();
    QGraphicsEllipseItem::hoverEnterEvent(event);
}

void PointHandle::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    isHovered = false;
    update();
    QGraphicsEllipseItem::hoverLeaveEvent(event);
}

void PointHandle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QColor color = amSelected ? Qt::yellow : Qt::white;
    if (isHovered)
        color = color.darker(130);
    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(color);
    painter->drawEllipse(rect());
}

void PointHandle::SelectMe(bool selected)
{
    amSelected = selected;
    update();
}
