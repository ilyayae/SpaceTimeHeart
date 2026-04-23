#ifndef MINDMAPVIEW_H
#define MINDMAPVIEW_H

#include <QGraphicsView>
#include <QObject>
#include <QUuid>
#include <QTimer>
#include <QRandomGenerator>
#include <QQueue>
#include <QFileInfo>
#include <QWheelEvent>
#include <include/mindMapObjects/mindmapnode.h>
class MindMapView : public QGraphicsView
{
    Q_OBJECT
public:
    MindMapView(QWidget *parent = nullptr, QList<QUuid> uuids = {}, QList<QString> paths = {}, QList<QList<QUuid>> connections = {});
    void nodeForceLayout();
    QList<QUuid> Uuids;
    QList<QString> Paths;
    QList<QList<QUuid>> Connections;
    QMap<QUuid, MindMapNode*> Nodes;
    void wheelEvent(QWheelEvent *event) override;
    QTimer *simTimer;
    int simStep = 0;

signals:
    void Clicked(QUuid uuid);

private:
    static constexpr int SIM_STEPS = 100;
    static constexpr double REPULSION = 80000.0;
    static constexpr double SPRING = 0.1;
    static constexpr double IDEAL_LENGTH = 200.0;
};

#endif // MINDMAPVIEW_H
