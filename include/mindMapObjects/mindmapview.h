#ifndef MINDMAPVIEW_H
#define MINDMAPVIEW_H

#include <QGraphicsView>
#include <QObject>
#include <QUuid>
#include <include/mindMapObjects/mindmapnode.h>
class MindMapView : public QGraphicsView
{
    Q_OBJECT
public:
    MindMapView(QWidget *parent = nullptr, QList<QUuid> uuids = {}, QList<QString> paths = {}, QList<QList<QUuid>> connections = {});
    QList<QUuid> Uuids;
    QList<QString> Paths;
    QList<QList<QUuid>> Connections;
    QList<MindMapNode*> Nodes;

};

#endif // MINDMAPVIEW_H
