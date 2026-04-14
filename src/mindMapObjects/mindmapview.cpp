#include "include/mindMapObjects/mindmapview.h"

MindMapView::MindMapView(QWidget *parent, QList<QUuid> uuids, QList<QString> paths, QList<QList<QUuid>> connections)
    : QGraphicsView(parent)
{
    Uuids = uuids;
    Paths = paths;
    Connections = connections;

    for(int i = 0; i < Uuids.count(); i++)
    {
        MindMapNode *node = new MindMapNode(nullptr, Uuids[i], Paths[i], Connections[i]);
        Nodes.append(node);
    }
    for(MindMapNode* node : Nodes)
    {
        node->Connect();
    }
}
