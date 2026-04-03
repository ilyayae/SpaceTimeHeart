#include "include/imageAnnotationObjects/markeritem.h"
#include "qgraphicsscene.h"
#include "qgraphicsview.h"

MarkerItem::MarkerItem(MarkerData *data, QGraphicsItem *parent)
    : QGraphicsObject(parent)
{
    myData = data;
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setFlag(ItemIgnoresTransformations);
}

void MarkerItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    prepareGeometryChange();
    isHovered = true;
    if (scene() && !scene()->views().isEmpty())
        scene()->views().first()->viewport()->update();
    QGraphicsObject::hoverEnterEvent(event);
}

void MarkerItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    prepareGeometryChange();
    isHovered = false;
    if (scene() && !scene()->views().isEmpty())
        scene()->views().first()->viewport()->update();
    QGraphicsObject::hoverLeaveEvent(event);
}

void MarkerItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit preClick();
        shift = event->modifiers() & Qt::ShiftModifier;
        pressPos = event->scenePos();
        isDragging = false;
        clickEmittedOnPress = false;
        if (!isSelected)
        {
            clickEmittedOnPress = true;
            emit clicked(this, shift);
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        emit rightClicked(this);
    }
    event->accept();
}

void MarkerItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        if (!isDragging && QLineF(event->scenePos(), pressPos).length() > 5)
            isDragging = true;
        if (isDragging)
            emit dragged(event->scenePos() - pressPos);
    }
    QGraphicsObject::mouseMoveEvent(event);
}

void MarkerItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (isDragging)
        {
            emit draggingFinished(event->scenePos() - pressPos);
            isDragging = false;
        }
        else if (!clickEmittedOnPress)
        {
            emit clicked(this, shift);
        }
        clickEmittedOnPress = false;
    }
    emit interactionFinished();
    QGraphicsObject::mouseReleaseEvent(event);
}

void MarkerItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (cachedPixmap.isNull())
    {
        cachedPixmap = QPixmap(ICON_SIZE, ICON_SIZE);
        cachedPixmap.fill(Qt::transparent);
        QPainter p(&cachedPixmap);
        QSvgRenderer renderer(QString(":/icons/markers/") + myData->IconId + ".svg");
        renderer.render(&p);
        p.end();

        QPixmap tinted(ICON_SIZE, ICON_SIZE);
        tinted.fill(Qt::transparent);
        QPainter tp(&tinted);
        tp.drawPixmap(0, 0, cachedPixmap);
        tp.setCompositionMode(QPainter::CompositionMode_Multiply);
        tp.fillRect(tinted.rect(), QColor(myData->Color));
        tp.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        tp.drawPixmap(0, 0, cachedPixmap);
        tp.end();
        cachedPixmap = tinted;
    }

    painter->drawPixmap(QRect(-myData->size/2, -myData->size/2, myData->size, myData->size), cachedPixmap);

    if (isSelected)
        painter->fillRect(boundingRect(), QColor(255, 255, 0, 60));

    if (isHovered){
        QFont font = painter->font();
        font.setPointSize(14);
        painter->setFont(font);
        QFontMetrics fm(font);
        int textWidth = fm.horizontalAdvance(myData->Label);
        int textX = -textWidth / 2;
        int textY = myData->size/2 + fm.ascent() + 4;
        QPen pen(Qt::black);
        QPainterPath outline;
        outline.addText(textX, textY, font, myData->Label);
        pen.setWidth(2);
        painter->strokePath(outline, pen);
        painter->setPen(Qt::white);
        painter->drawText(textX, textY, myData->Label);
    }
}

QRectF MarkerItem::boundingRect() const
{
    return QRectF(-myData->size/2, -myData->size/2, myData->size, myData->size);
}

void MarkerItem::SelectMe(bool selected)
{
    isSelected = selected;
    if (scene() && !scene()->views().isEmpty())
        scene()->views().first()->viewport()->update();
}
