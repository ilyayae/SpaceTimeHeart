#ifndef CUSTOMGRAPHICSVIEW_H
#define CUSTOMGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QObject>
#include <QWidget>
#include <QWheelEvent>

class CustomGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    CustomGraphicsView(QWidget *parent);
    void wheelEvent(QWheelEvent *event) override;
};

#endif // CUSTOMGRAPHICSVIEW_H
