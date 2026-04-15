#ifndef MINDMAPEDGE_H
#define MINDMAPEDGE_H

#include <QGraphicsItem>

class MindMapNode;

class MindMapEdge : public QGraphicsItem
{
public:
    MindMapEdge(MindMapNode *from, MindMapNode *to);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    MindMapNode *From = nullptr;
    MindMapNode *To = nullptr;
    bool highlighted = false;
};

#endif // MINDMAPEDGE_H
