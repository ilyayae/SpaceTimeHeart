#ifndef CUSTOMGRAPHICSVIEW_H
#define CUSTOMGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QObject>
#include <QWidget>
#include <QWheelEvent>
#include <QPoint>
#include <QScrollBar>


class CustomGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    CustomGraphicsView(QWidget *parent);
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool isPanning = false;
    bool panningLocked = false;
    QPoint lastPanPos;
};

#endif // CUSTOMGRAPHICSVIEW_H
