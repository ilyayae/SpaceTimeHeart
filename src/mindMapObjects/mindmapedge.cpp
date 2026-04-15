#include "include/mindMapObjects/mindmapedge.h"
#include "include/mindMapObjects/mindmapnode.h"

MindMapEdge::MindMapEdge(MindMapNode *from, MindMapNode *to)
{
    //
    From = from;
    To = to;
    setZValue(0);
}

QRectF MindMapEdge::boundingRect() const
{
    QPointF src = From->pos();
    QPointF tgt = To->pos();

    QPointF delta = tgt - src;
    qreal length = qMax(qSqrt(delta.x()*delta.x() + delta.y()*delta.y()), 1.0);
    QPointF perp(-delta.y() / length, delta.x() / length);
    QPointF offset = perp * 20.0;
    QPointF srcOff = src + offset;
    QPointF tgtOff = tgt + offset;
    qreal padding = 15;
    return QRectF(srcOff, tgtOff).normalized().adjusted(-padding, -padding, padding, padding);
}

void MindMapEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{

    painter->setRenderHint(QPainter::Antialiasing);

    QPointF src = From->pos();
    QPointF tgt = To->pos();

    QPointF delta = tgt - src;
    qreal length = qMax(qSqrt(delta.x()*delta.x() + delta.y()*delta.y()), 1.0);
    QPointF perp(-delta.y() / length, delta.x() / length);
    qreal offsetAmount = 20.0;
    QPointF offset = perp * offsetAmount;

    QPointF srcOff = src + offset;
    QPointF tgtOff = tgt + offset;

    QPointF ctrl1(srcOff.x() + (tgtOff.x() - srcOff.x()) * 0.5, srcOff.y());
    QPointF ctrl2(srcOff.x() + (tgtOff.x() - srcOff.x()) * 0.5, tgtOff.y());

    QPainterPath path;
    path.moveTo(src);
    path.cubicTo(ctrl1, ctrl2, tgt);

    QColor color = highlighted ? From->color.lighter(110) : From->color;
    qreal width = highlighted ? 10 : 5;

    painter->setPen(QPen(color, width, Qt::SolidLine, Qt::RoundCap));
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path);
}
