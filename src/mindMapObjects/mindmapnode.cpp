#include "include/mindMapObjects/mindmapnode.h"

MindMapNode::MindMapNode(QGraphicsItem *parent, QUuid myUuid, QString myName, QList<QUuid> myConnections)
    : QGraphicsObject(parent)
{
    MyUuid = myUuid;
    MyName = myName;
    MyConnections = myConnections;
}

QRectF MindMapNode::boundingRect() const
{
    qreal w = BASE_WIDTH  * m_nodeScale;
    qreal h = BASE_HEIGHT * m_nodeScale;
    return QRectF(-w / 2, -h / 2, w, h);
}

void MindMapNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);

    QRectF rect = boundingRect();

    // Drop shadow
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0, 0, 0, 60));
    painter->drawRoundedRect(rect.translated(3, 3), 10, 10);

    // Node body
    painter->setBrush(m_color);
    painter->setPen(QPen(m_color.darker(130), 2));
    painter->drawRoundedRect(rect, 10, 10);

    // Label
    QColor textColor = m_color.lightness() > 160 ? Qt::black : Qt::white;
    painter->setPen(textColor);
    painter->setFont(QFont("Segoe UI", 9, QFont::Medium));
    painter->drawText(rect, Qt::AlignCenter | Qt::TextWordWrap, m_name);
}
