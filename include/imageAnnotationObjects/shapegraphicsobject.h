#ifndef SHAPEGRAPHICSOBJECT_H
#define SHAPEGRAPHICSOBJECT_H

#include "include/noteTypes/imageannotationdata.h"
#include "include/imageAnnotationObjects/pointhandle.h"
#include <QGraphicsObject>
#include <QUndoStack>
#include <QGraphicsSceneMouseEvent>

class ShapeGraphicsObject : public QGraphicsObject
{
    Q_OBJECT
public:
    ShapeGraphicsObject(ImageAnnotationData *data, int shapeIndex, QGraphicsPixmapItem *image, QUndoStack *stack);
    ImageAnnotationData *Data;
    int ShapeIndex;
    ~ShapeGraphicsObject();
    void onPointMoved(PointHandle *point, QPair<double, double>);
    void onPointClicked(PointHandle *point, bool clicked);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void syncFromData();
    void setHandlesVisible(bool visible = true);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    ShapeData* MyData() const;
    bool editing = false;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    bool sceneEventFilter(QGraphicsItem *watched, QEvent *event) override;
    QList<PointHandle*> handles;
    QGraphicsPixmapItem *Image;
    QUndoStack *Stack;
private slots:

signals:
    void hovered(ShapeGraphicsObject *me, bool hovered);
    void rightClicked(ShapeGraphicsObject *me);
    void handleHeld(bool held);

};

#endif // SHAPEGRAPHICSOBJECT_H
