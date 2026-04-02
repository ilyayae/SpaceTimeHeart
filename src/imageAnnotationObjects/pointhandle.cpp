#include "include/imageAnnotationObjects/pointhandle.h"
#include "include/imageAnnotationObjects/shapegraphicsobject.h"

PointHandle::PointHandle(ShapeGraphicsObject *parent, int id)
    : Parent(parent), Id(id)
{
    setRect((Parent->MyData->XYPoints[Id].first * Parent->Image->pixmap().width()) - SIZE/2.0,
            (Parent->MyData->XYPoints[Id].second * Parent->Image->pixmap().height()) - SIZE/2.0,
            SIZE,
            SIZE);
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIgnoresTransformations);
    setAcceptHoverEvents(true);
}

QVariant PointHandle::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()) {
        QPointF newPos = value.toPointF();
        QRectF rect = scene()->sceneRect();
        if (!rect.contains(newPos)) {
            newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
            newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
            return newPos;
        }
    }
    Parent->update();
    return QGraphicsItem::itemChange(change, value);
}

QPair<double, double> PointHandle::GetPosition()
{
    return QPair<double, double>(
        (rect().center().x() + pos().x()) / Parent->Image->pixmap().width(),
        (rect().center().y() + pos().y()) / Parent->Image->pixmap().height()
        );
}

void PointHandle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    storedPosition = GetPosition();
    QGraphicsEllipseItem::mousePressEvent(event);
}
void PointHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(storedPosition != GetPosition())
    {
        Parent->onPointMoved(this, storedPosition);
    }
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
