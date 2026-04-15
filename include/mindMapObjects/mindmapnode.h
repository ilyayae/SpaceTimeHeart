#ifndef MINDMAPNODE_H
#define MINDMAPNODE_H

#include <QGraphicsObject>
#include <QObject>
#include <QUuid>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <include/mindMapObjects/mindmapedge.h>

class MindMapNode : public QGraphicsObject
{
    Q_OBJECT
public:
    MindMapNode(QGraphicsItem *parent = nullptr, QUuid myUuid = QUuid(), QString myName = "", QList<QUuid> myConnections = {});
    void Connect(QMap<QUuid, MindMapNode*> *nodesMap);
    QUuid MyUuid;
    QString MyName;
    QList<QUuid> MyConnections;
    QList<MindMapEdge*> MyEdges;
    QColor color;
    bool amHovered = false;
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void setHighlightEdges(bool highlighted);

signals:
    void Clicked(QUuid uuid);

private:
    static constexpr int BASE_SIZE = 128;
};

#endif // MINDMAPNODE_H
