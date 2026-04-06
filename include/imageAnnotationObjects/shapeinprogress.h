#ifndef SHAPEINPROGRESS_H
#define SHAPEINPROGRESS_H

#include <QGraphicsObject>
#include <QObject>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>

class ShapeInProgress : public QGraphicsObject
{
    Q_OBJECT
public:
    ShapeInProgress(QPoint point, QColor *currentLineColor, QColor *currentFillColor, Qt::PenStyle *currentPenStyle,
                    Qt::BrushStyle *currentBrushStyle, int *currentRounding, int *currentWidth, QGraphicsPixmapItem *image);
    ShapeInProgress(QPair<double, double> pair, QColor *currentLineColor, QColor *currentFillColor, Qt::PenStyle *currentPenStyle,
                    Qt::BrushStyle *currentBrushStyle, int *currentRounding, int *currentWidth, QGraphicsPixmapItem *image);
    QColor *CurrentLineColor;
    QColor *CurrentFillColor;
    Qt::PenStyle *CurrentPenStyle;
    Qt::BrushStyle *CurrentBrushStyle;
    int *CurrentRounding;
    int *CurrentWidth;
    QList<QPair<double, double>> XYPoints;
    QGraphicsPixmapItem *Image;
    QPointF mousePos;

    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QRectF boundingRect() const override;
    void addPoint(QPair<double, double>);
    void addPoint(QPoint point);
};

#endif // SHAPEINPROGRESS_H
