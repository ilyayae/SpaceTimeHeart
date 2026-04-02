#ifndef POINTHANDLE_H
#define POINTHANDLE_H

#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
class ShapeGraphicsObject;


class PointHandle : public QGraphicsEllipseItem
{
public:
    int SIZE = 16;
    PointHandle(ShapeGraphicsObject *parent, int id);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    QPair<double, double> GetPosition();
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void SelectMe(bool selected);

    ShapeGraphicsObject *Parent;
    QPair<double, double> storedPosition;
    int Id;
    bool amSelected = false;
    bool isHovered = false;
private:
signals:
    void moved(PointHandle *me, QPair<double, double> storedPosition);
    void clicked(PointHandle *me);
};

#endif // POINTHANDLE_H
