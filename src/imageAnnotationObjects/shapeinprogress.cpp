#include "include/imageAnnotationObjects/shapeinprogress.h"
#include "qpen.h"

ShapeInProgress::ShapeInProgress(QPoint point, QColor *currentLineColor, QColor *currentFillColor, Qt::PenStyle *currentPenStyle,
                                 Qt::BrushStyle *currentBrushStyle, int *currentRounding, int *currentWidth, QGraphicsPixmapItem *image)
    : CurrentLineColor(currentLineColor), CurrentFillColor(currentFillColor), CurrentPenStyle(currentPenStyle),
    CurrentBrushStyle(currentBrushStyle), CurrentRounding(currentRounding),  CurrentWidth(currentWidth), Image(image)
{
    setAcceptHoverEvents(true);
    addPoint(point);
    scene()->installEventFilter(this);
}

ShapeInProgress::ShapeInProgress(QPair<double, double> pair, QColor *currentLineColor, QColor *currentFillColor, Qt::PenStyle *currentPenStyle,
                                 Qt::BrushStyle *currentBrushStyle, int *currentRounding, int *currentWidth, QGraphicsPixmapItem *image)
    : CurrentLineColor(currentLineColor), CurrentFillColor(currentFillColor), CurrentPenStyle(currentPenStyle),
    CurrentBrushStyle(currentBrushStyle), CurrentRounding(currentRounding),  CurrentWidth(currentWidth), Image(image)
{
    setAcceptHoverEvents(true);
    addPoint(pair);
}

void ShapeInProgress::addPoint(QPoint point)
{
    QPair<double, double> pair(point.x(), point.y());
    addPoint(pair);
}

void ShapeInProgress::addPoint(QPair<double, double> pair)
{
    XYPoints.append(pair);
}

void ShapeInProgress::movedMouseUpdate(QPointF where)
{
    mousePos = where;
    update();
}
void ShapeInProgress::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (XYPoints.isEmpty())
        return;

    QList<QPair<double, double>> points = XYPoints;
    points.append(QPair<double, double>(
        mousePos.x() / Image->pixmap().width(),
        mousePos.y() / Image->pixmap().height()
        ));

    QPainterPath path;
    double x0 = points[0].first * Image->pixmap().width();
    double y0 = points[0].second * Image->pixmap().height();

    if (*CurrentRounding > 0 && points.count() >= 3)
    {
        double rounding = *CurrentRounding / 100.0;
        for (int i = 0; i < points.count(); i++)
        {
            double X = points[i].first * Image->pixmap().width();
            double Y = points[i].second * Image->pixmap().height();
            int prevIdx = (i == 0) ? points.count() - 1 : i - 1;
            int nextIdx = (i + 1) % points.count();
            double px = points[prevIdx].first * Image->pixmap().width();
            double py = points[prevIdx].second * Image->pixmap().height();
            double nx = points[nextIdx].first * Image->pixmap().width();
            double ny = points[nextIdx].second * Image->pixmap().height();
            double startX = X + (px - X) * rounding;
            double startY = Y + (py - Y) * rounding;
            double endX = X + (nx - X) * rounding;
            double endY = Y + (ny - Y) * rounding;
            if (i == 0)
            {
                path.moveTo(X, Y);
                path.lineTo(endX, endY);
            }
            else if (i == points.count() - 1)
            {
                path.lineTo(X, Y);
            }
            else
            {
                path.lineTo(startX, startY);
                path.quadTo(X, Y, endX, endY);
            }
        }
    }
    else
    {
        path.moveTo(x0, y0);
        for (int i = 1; i < points.count(); i++)
        {
            double x = points[i].first * Image->pixmap().width();
            double y = points[i].second * Image->pixmap().height();
            path.lineTo(x, y);
        }
    }

    QPen pen(*CurrentLineColor, *CurrentWidth, *CurrentPenStyle);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path);
}

QRectF ShapeInProgress::boundingRect() const
{
    return QRectF(0, 0, Image->pixmap().width(), Image->pixmap().height());
}

