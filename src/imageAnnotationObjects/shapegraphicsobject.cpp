#include "include/imageAnnotationObjects/shapegraphicsobject.h"

ShapeGraphicsObject::ShapeGraphicsObject(ShapeData *myData, QGraphicsPixmapItem *image, QUndoStack *stack)
    : MyData(myData), Image(image), Stack(stack)
{
    for(int i = 0; i < MyData->XYPoints.count(); i++)
    {
        PointHandle *ph = new PointHandle(this, i);
        ph->setParentItem(this);
        ph->hide();
        handles.append(ph);
    }
}
ShapeGraphicsObject::~ShapeGraphicsObject()
{

}

Qt::PenStyle penStyleFromString(const QString &style)
{
    if (style == "Dash") return Qt::DashLine;
    if (style == "Dot") return Qt::DotLine;
    if (style == "Dash-Dot") return Qt::DashDotLine;
    if (style == "Dash-Dot-Dot") return Qt::DashDotDotLine;
    return Qt::SolidLine;
}
Qt::BrushStyle brushStyleFromString(const QString &style)
{
    if (style == "None") return Qt::NoBrush;
    if (style == "Dense") return Qt::Dense4Pattern;
    if (style == "Horizontal") return Qt::HorPattern;
    if (style == "Vertical") return Qt::VerPattern;
    if (style == "Cross") return Qt::CrossPattern;
    if (style == "Diagonal") return Qt::BDiagPattern;
    return Qt::SolidPattern;
}
void ShapeGraphicsObject::onPointMoved(PointHandle *point, QPair<double, double>)
{
    MovePointCommand *mpc = new MovePointCommand(MyData, point->Id, point->GetPosition());
    Stack->push(mpc);
    syncFromData();
}
void ShapeGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (handles.count() < 2)
        return;

    QPainterPath path;
    double x0 = MyData->XYPoints[0].first * Image->pixmap().width();
    double y0 = MyData->XYPoints[0].second * Image->pixmap().height();

    if (MyData->rounding > 0 && handles.count() >= 3)
    {
        double rounding = MyData->rounding / 100.0;

        for (int i = 0; i < handles.count(); i++)
        {
            double cx = MyData->XYPoints[i].first * Image->pixmap().width();
            double cy = MyData->XYPoints[i].second * Image->pixmap().height();

            int prevIdx = (i == 0) ? handles.count() - 1 : i - 1;
            int nextIdx = (i + 1) % handles.count();

            double px = MyData->XYPoints[prevIdx].first * Image->pixmap().width();
            double py = MyData->XYPoints[prevIdx].second * Image->pixmap().height();
            double nx = MyData->XYPoints[nextIdx].first * Image->pixmap().width();
            double ny = MyData->XYPoints[nextIdx].second * Image->pixmap().height();

            double startX = cx + (px - cx) * rounding;
            double startY = cy + (py - cy) * rounding;
            double endX = cx + (nx - cx) * rounding;
            double endY = cy + (ny - cy) * rounding;

            if (i == 0)
                path.moveTo(startX, startY);
            else
                path.lineTo(startX, startY);

            path.quadTo(cx, cy, endX, endY);
        }

        if (MyData->Closed)
        {
            double cx = MyData->XYPoints[0].first * Image->pixmap().width();
            double cy = MyData->XYPoints[0].second * Image->pixmap().height();
            double startX = cx + (MyData->XYPoints[handles.count() - 1].first * Image->pixmap().width() - cx) * rounding;
            double startY = cy + (MyData->XYPoints[handles.count() - 1].second * Image->pixmap().height() - cy) * rounding;
            path.lineTo(startX, startY);
            path.closeSubpath();
        }
    }
    else
    {
        path.moveTo(x0, y0);
        for (int i = 1; i < handles.count(); i++)
        {
            double x = MyData->XYPoints[i].first * Image->pixmap().width();
            double y = MyData->XYPoints[i].second * Image->pixmap().height();
            path.lineTo(x, y);
        }

        if (MyData->Closed)
            path.closeSubpath();
    }

    QPen pen(QColor(MyData->StyleOfLine.lineColor), MyData->StyleOfLine.width, penStyleFromString(MyData->StyleOfLine.linePatternId));
    painter->setPen(pen);

    if (MyData->Closed)
        painter->setBrush(QBrush(MyData->StyleOfFill.fillColor, brushStyleFromString(MyData->StyleOfFill.fillPatternId)));
    else
        painter->setBrush(Qt::NoBrush);

    painter->drawPath(path);
}
void ShapeGraphicsObject::syncFromData()
{
    prepareGeometryChange();

    while (handles.count() > MyData->XYPoints.count())
    {
        PointHandle *h = handles.takeLast();
        scene()->removeItem(h);
        delete h;
    }

    while (handles.count() < MyData->XYPoints.count())
    {
        PointHandle *h = new PointHandle(this, handles.count());
        handles.append(h);
        scene()->addItem(h);
    }

    for (int i = 0; i < handles.count(); i++)
    {
        double x = MyData->XYPoints[i].first * Image->pixmap().width();
        double y = MyData->XYPoints[i].second * Image->pixmap().height();
        handles[i]->setPos(0, 0);
        handles[i]->setRect(x - handles[i]->SIZE / 2.0, y - handles[i]->SIZE / 2.0, handles[i]->SIZE, handles[i]->SIZE);
    }

    update();
}
void ShapeGraphicsObject::setHandlesVisible(bool visible)
{
    editing = visible;
    if(visible)
    {
        for(int i = 0; i < handles.count(); i++)
        {
            handles[i]->show();
        }
    }
    else
    {
        for(int i = 0; i < handles.count(); i++)
        {
            handles[i]->hide();
        }

    }
}
void ShapeGraphicsObject::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    emit hovered(this, true);
    QGraphicsObject::hoverEnterEvent(event);
}
void ShapeGraphicsObject::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    emit hovered(this, false);
    QGraphicsObject::hoverLeaveEvent(event);
}
QRectF ShapeGraphicsObject::boundingRect() const
{
    QRectF result;
    for(int i = 0; i < handles.count(); i++)
    {
        double x = MyData->XYPoints[i].first * Image->pixmap().width();
        double y = MyData->XYPoints[i].second * Image->pixmap().height();
        if (i == 0)
            result = QRectF(x, y, 0, 0);
        else
            result = result.united(QRectF(x, y, 0, 0));
    }
    double margin = MyData->StyleOfLine.width / 2.0 + 1.0;
    return result.adjusted(-margin, -margin, margin, margin);
}
QPainterPath ShapeGraphicsObject::shape() const
{    QPainterPath path;

    if (handles.count() < 2)
        return path;

    double x0 = MyData->XYPoints[0].first * Image->pixmap().width();
    double y0 = MyData->XYPoints[0].second * Image->pixmap().height();

    path.moveTo(x0, y0);
    for (int i = 1; i < handles.count(); i++)
    {
        double x = MyData->XYPoints[i].first * Image->pixmap().width();
        double y = MyData->XYPoints[i].second * Image->pixmap().height();
        path.lineTo(x, y);
    }

    if (MyData->Closed)
        path.closeSubpath();

    QPainterPathStroker stroker;
    stroker.setWidth(MyData->StyleOfLine.width + 6.0);
    return stroker.createStroke(path);
}

void ShapeGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(editing)
    {
        if(event->button() == Qt::RightButton)
        {
            emit rightClicked(this);
        }
    }
    QGraphicsObject::mousePressEvent(event);
}
void ShapeGraphicsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsObject::mouseReleaseEvent(event);
}
