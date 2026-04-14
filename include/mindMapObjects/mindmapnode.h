#ifndef MINDMAPNODE_H
#define MINDMAPNODE_H

#include <QGraphicsObject>
#include <QObject>
#include <QUuid>
#include <include/mindMapObjects/mindmapedge.h>

class MindMapNode : public QGraphicsObject
{
    Q_OBJECT
public:
    MindMapNode(QGraphicsItem *parent = nullptr, QUuid myUuid = QUuid(), QString myName = "", QList<QUuid> myConnections = {});
    void Connect();
    QUuid MyUuid;
    QString MyName;
    QList<QUuid> MyConnections;

    QList<MindMapEdge*> MyEdges;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

};

#endif // MINDMAPNODE_H
