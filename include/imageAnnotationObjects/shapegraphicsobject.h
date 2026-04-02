#ifndef SHAPEGRAPHICSOBJECT_H
#define SHAPEGRAPHICSOBJECT_H

#include "include/noteTypes/imageannotationdata.h"
#include "include/imageAnnotationObjects/pointhandle.h"
#include "include/imageAnnotationObjects/vectorpaintercommands.h"
#include <QGraphicsObject>
#include <QUndoStack>
#include <QGraphicsSceneMouseEvent>

class ShapeGraphicsObject : public QGraphicsObject
{
    Q_OBJECT
public:
    ShapeGraphicsObject(ShapeData *myData, QGraphicsPixmapItem *image, QUndoStack *stack);
    ~ShapeGraphicsObject();
    void onPointMoved(PointHandle *point, QPair<double, double>);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void syncFromData();
    void setHandlesVisible(bool visible = true);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    bool editing = false;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    QList<PointHandle*> handles;
    ShapeData *MyData;
    QGraphicsPixmapItem *Image;
    QUndoStack *Stack;
private slots:

signals:
    void hovered(ShapeGraphicsObject *me, bool hovered);
    void rightClicked(ShapeGraphicsObject *me);

};

#endif // SHAPEGRAPHICSOBJECT_H
