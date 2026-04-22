#include "include/imageAnnotationObjects/shapegraphicsobject.h"
#include "include/imageAnnotationObjects/vectorpaintercommands.h"

ShapeGraphicsObject::ShapeGraphicsObject(ImageAnnotationData *data, int shapeIndex, QGraphicsItem *image, QUndoStack *stack)
    : Data(data), ShapeIndex(shapeIndex), Image(image), Stack(stack)
{
    for(int i = 0; i < MyData()->XYPoints.count(); i++)
    {
        PointHandle *ph = new PointHandle(this, i);
        ph->setParentItem(this);
        ph->hide();
        handles.append(ph);
    }
    setAcceptHoverEvents(true);
    setFiltersChildEvents(true);
}
ShapeGraphicsObject::~ShapeGraphicsObject()
{

}

ShapeData* ShapeGraphicsObject::MyData() const
{
    return &Data->shapes[ShapeIndex];
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
    MovePointCommand *mpc = new MovePointCommand(Data, ShapeIndex, point->Id, point->GetPosition());
    Stack->push(mpc);
    prepareGeometryChange();
    update();
}

void ShapeGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (handles.count() < 2)
        return;

    QPainterPath path;
    double x0 = MyData()->XYPoints[0].first * Image->boundingRect().width();
    double y0 = MyData()->XYPoints[0].second * Image->boundingRect().height();

    if (MyData()->rounding > 0 && handles.count() >= 3)
    {
        double rounding = MyData()->rounding / 100.0;

        for (int i = 0; i < handles.count(); i++)
        {
            double X = MyData()->XYPoints[i].first * Image->boundingRect().width(); // X Position on the image for this point.
            double Y = MyData()->XYPoints[i].second * Image->boundingRect().height(); // Y Position on the image for this point.

            int prevIdx = (i == 0) ? handles.count() - 1 : i - 1;
            int nextIdx = (i + 1) % handles.count();

            double px = MyData()->XYPoints[prevIdx].first * Image->boundingRect().width();
            double py = MyData()->XYPoints[prevIdx].second * Image->boundingRect().height();
            double nx = MyData()->XYPoints[nextIdx].first * Image->boundingRect().width();
            double ny = MyData()->XYPoints[nextIdx].second * Image->boundingRect().height();

            double startX = X + (px - X) * rounding;
            double startY = Y + (py - Y) * rounding;
            double endX = X + (nx - X) * rounding;
            double endY = Y + (ny - Y) * rounding;

            if (i == 0 && !MyData()->Closed)
            {
                path.moveTo(X, Y);
                path.lineTo(endX, endY);
            }
            else if (i == handles.count() - 1 && !MyData()->Closed)
            {
                path.lineTo(X, Y);
            }
            else
            {
                if (i == 0)
                    path.moveTo(startX, startY);
                else
                    path.lineTo(startX, startY);
                path.quadTo(X, Y, endX, endY);
            }
        }

        if (MyData()->Closed)
        {
            double cx = MyData()->XYPoints[0].first * Image->boundingRect().width();
            double cy = MyData()->XYPoints[0].second * Image->boundingRect().height();
            double startX = cx + (MyData()->XYPoints[handles.count() - 1].first * Image->boundingRect().width() - cx) * rounding;
            double startY = cy + (MyData()->XYPoints[handles.count() - 1].second * Image->boundingRect().height() - cy) * rounding;
            path.lineTo(startX, startY);
            path.closeSubpath();
        }
    }
    else
    {
        path.moveTo(x0, y0);
        for (int i = 1; i < handles.count(); i++)
        {
            double x = MyData()->XYPoints[i].first * Image->boundingRect().width();
            double y = MyData()->XYPoints[i].second * Image->boundingRect().height();
            path.lineTo(x, y);
        }

        if (MyData()->Closed)
            path.closeSubpath();
    }

    QPen pen(QColor(MyData()->StyleOfLine.lineColor), MyData()->StyleOfLine.width, penStyleFromString(MyData()->StyleOfLine.linePatternId));
    painter->setPen(pen);

    if (MyData()->Closed)
        painter->setBrush(QBrush(MyData()->StyleOfFill.fillColor, brushStyleFromString(MyData()->StyleOfFill.fillPatternId)));
    else
        painter->setBrush(Qt::NoBrush);

    painter->drawPath(path);
}

void ShapeGraphicsObject::syncFromData()
{
    prepareGeometryChange();

    while (handles.count() > MyData()->XYPoints.count())
    {
        PointHandle *h = handles.takeLast();
        scene()->removeItem(h);
        delete h;
    }

    while (handles.count() < MyData()->XYPoints.count())
    {
        PointHandle *h = new PointHandle(this, handles.count());
        handles.append(h);
        scene()->addItem(h);
    }

    for (int i = 0; i < handles.count(); i++)
    {
        double x = MyData()->XYPoints[i].first * Image->boundingRect().width();
        double y = MyData()->XYPoints[i].second * Image->boundingRect().height();
        handles[i]->setPos(x, y);
        handles[i]->setRect(-handles[i]->SIZE / 2.0, -handles[i]->SIZE / 2.0, handles[i]->SIZE, handles[i]->SIZE);
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
    return QRectF(0, 0, Image->boundingRect().width(), Image->boundingRect().height());
}

QPainterPath ShapeGraphicsObject::shape() const
{
    QPainterPath path;

    if (handles.count() < 2)
        return path;

    double x0 = MyData()->XYPoints[0].first * Image->boundingRect().width();
    double y0 = MyData()->XYPoints[0].second * Image->boundingRect().height();

    path.moveTo(x0, y0);
    for (int i = 1; i < handles.count(); i++)
    {
        double x = MyData()->XYPoints[i].first * Image->boundingRect().width();
        double y = MyData()->XYPoints[i].second * Image->boundingRect().height();
        path.lineTo(x, y);
    }

    if (MyData()->Closed)
    {
        path.closeSubpath();
        QPainterPathStroker stroker;
        stroker.setWidth(MyData()->StyleOfLine.width + 6.0);
        return path.united(stroker.createStroke(path));
    }

    QPainterPathStroker stroker;
    stroker.setWidth(MyData()->StyleOfLine.width + 6.0);
    return stroker.createStroke(path);
}

void ShapeGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
        if(event->button() == Qt::RightButton)
        {
            emit rightClicked(this);
        }
    QGraphicsObject::mousePressEvent(event);
}

void ShapeGraphicsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsObject::mouseReleaseEvent(event);
}

bool ShapeGraphicsObject::sceneEventFilter(QGraphicsItem *watched, QEvent *event)
{
    if (event->type() == QEvent::GraphicsSceneHoverEnter ||
        event->type() == QEvent::GraphicsSceneHoverLeave)
    {
        return true;
    }
    return false;
}

void ShapeGraphicsObject::onPointClicked(PointHandle *point, bool clicked)
{
    emit handleHeld(clicked);
}
