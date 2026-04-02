#ifndef MARKERITEM_H
#define MARKERITEM_H

#include <QGraphicsObject>
#include <QPixmap>
#include <QPainter>
#include <QSvgRenderer>
#include <QRectF>
#include <QGraphicsSceneMouseEvent>
#include <include/noteTypes/imageannotationdata.h>

constexpr int ICON_SIZE = 256;

class MarkerItem : public QGraphicsObject
{
    Q_OBJECT
public:
    MarkerItem(MarkerData *data = nullptr, QGraphicsItem *parent = nullptr);
    MarkerData *myData;
    QRectF boundingRect() const override;
    void paint(QPainter *painter = nullptr, const QStyleOptionGraphicsItem *option = nullptr, QWidget *widget = nullptr) override;
    bool isHovered = false;
    bool isSelected = false;
    bool isDragging = false;
    bool shift = false;
    bool clickEmittedOnPress = false;
    QPointF pressPos;
    QPixmap cachedPixmap;
    void SelectMe(bool selected);
protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
signals:
    void preClick();
    void interactionFinished();
    void clicked(MarkerItem *me, bool shift);
    void rightClicked(MarkerItem *me);
    void dragged(QPointF delta);
    void draggingFinished(QPointF delta);
};

#endif // MARKERITEM_H
