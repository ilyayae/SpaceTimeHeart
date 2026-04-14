#include "include/editors/mindmap.h"
#include "ui_mindmap.h"

MindMap::MindMap(QWidget *parent, UuidRegistry *reg)
    : QMainWindow(parent)
    , ui(new Ui::MindMap)
{
    ui->setupUi(this);
    myRegistry = reg;
}

MindMap::~MindMap()
{
    delete ui;
}

void MindMap::setUp()
{
    QList<QPair<QUuid, QString>> list = myRegistry->getAllUuids();
    QList<QUuid> uuids;
    QList<QString> paths;
    QList<QList<QUuid>> connections;
    for(QPair<QUuid, QString> pair : list)
    {
        uuids.append(pair.first);
        paths.append(pair.second);
        connections.append(myRegistry->getConnectedUuids(pair.first));
    }
    myView = new MindMapView(this, uuids, paths, connections);
    ui->centralwidget->layout()->addWidget(myView);
}
