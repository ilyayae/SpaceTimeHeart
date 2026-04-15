#include "include/mindMapObjects/mindmapnode.h"

MindMapNode::MindMapNode(QGraphicsItem *parent, QUuid myUuid, QString myName, QList<QUuid> myConnections)
    : QGraphicsObject(parent)
{
    MyUuid = myUuid;
    MyName = myName;
    MyConnections = myConnections;
    size_t hash = qHash(MyUuid.toString());
    int hue = hash % 360;
    color = QColor::fromHsvF(hue / 360.0, 0.6, 0.8);
    setZValue(1);
    setAcceptHoverEvents(true);
}

QRectF MindMapNode::boundingRect() const
{
    qreal pad = 10;
    return QRectF(-BASE_SIZE / 2 - pad, -BASE_SIZE / 2 - pad,
                  BASE_SIZE + pad * 2,  BASE_SIZE + pad * 2);
}

void MindMapNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);

    QRectF rect(-BASE_SIZE / 2, -BASE_SIZE / 2, BASE_SIZE, BASE_SIZE);

    if (amHovered)
        rect = rect.adjusted(-5, -5, 5, 5);

    painter->setBrush(amHovered ? color.lighter(110) : color);
    painter->setPen(QPen(color.darker(130), 2));
    painter->drawEllipse(amHovered ? rect.adjusted(-5, -5, 5, 5) : rect);

    QColor textColor = color.lightness() > 160 ? Qt::black : Qt::white;
    painter->setPen(textColor);
    painter->setFont(QFont("Segoe UI", 12, QFont::Medium));
    painter->drawText(rect, Qt::AlignCenter | Qt::TextWordWrap, MyName);
}

void MindMapNode::Connect(QMap<QUuid, MindMapNode*> *nodesMap)
{
    for(QUuid connectTo : MyConnections)
    {
        MindMapNode *to = nodesMap->value(connectTo);
        if(to != nullptr)
        {
            MindMapEdge *edge = new MindMapEdge(this, to);
            MyEdges.append(edge);
            scene()->addItem(edge);
        }
    }
}

void MindMapNode::setHighlightEdges(bool highlighted)
{
    for(MindMapEdge *ed : MyEdges)
    {
        ed->highlighted = highlighted;
        ed->update();
    }
    amHovered = highlighted;
    update();
}

void MindMapNode::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setHighlightEdges(true);
    QGraphicsObject::hoverEnterEvent(event);
}

void MindMapNode::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setHighlightEdges(false);
    QGraphicsObject::hoverLeaveEvent(event);
}

void MindMapNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit Clicked(MyUuid);
    }
    event->accept();
}
