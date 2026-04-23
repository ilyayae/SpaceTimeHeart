#include "include/mindMapObjects/mindmapview.h"

MindMapView::MindMapView(QWidget *parent, QList<QUuid> uuids, QList<QString> paths, QList<QList<QUuid>> connections)
    : QGraphicsView(parent)
{
    setScene(new QGraphicsScene(this));

    Uuids = uuids;
    Paths = paths;
    Connections = connections;
    int count = Uuids.count();
    for (int i = 0; i < count; i++)
    {
        QString fullPath = Paths[i];
        QString fileName = QFileInfo(fullPath).completeBaseName();
        MindMapNode *node = new MindMapNode(nullptr, Uuids[i], fileName, Connections[i]);
        connect(node, &MindMapNode::Clicked, this, [this](QUuid uuid){
            emit Clicked(uuid);
        });
        scene()->addItem(node);
        Nodes[Uuids[i]] = node;
    }
    for(MindMapNode* node : Nodes)
    {
        node->Connect(&Nodes);
    }

    QMap<MindMapNode*, int> circleIds; //Node belongs to what "circle"
    int currentCircle = 0;
    QList<QList<MindMapNode*>> circles;

    QMap<MindMapNode*, QList<MindMapNode*>> incomingConnections;
    for (MindMapNode* node : Nodes)
    {
        for (const QUuid &targetUuid : node->MyConnections)
        {
            if (Nodes.contains(targetUuid))
                incomingConnections[Nodes[targetUuid]].append(node);
        }
    }

    for (MindMapNode* startUuid : Nodes.values())
    {
        if (circleIds.contains(startUuid)) continue;

        QList<MindMapNode*> circle;
        QQueue<MindMapNode*> queue;
        queue.enqueue(startUuid);
        circleIds[startUuid] = currentCircle;

        while (!queue.isEmpty())
        {
            MindMapNode* current = queue.dequeue();
            circle.append(current);

            for (const QUuid &neighbor : current->MyConnections)
            {
                if (Nodes.contains(neighbor) && !circleIds.contains(Nodes[neighbor]))
                {
                    circleIds[Nodes[neighbor]] = currentCircle;
                    queue.enqueue(Nodes[neighbor]);
                }
            }


            for (MindMapNode* neighbor : incomingConnections.value(current))
            {
                if (!circleIds.contains(neighbor))
                {
                    circleIds[neighbor] = currentCircle;
                    queue.enqueue(neighbor);
                }
            }
        }

        circles.append(circle);
        currentCircle++;
    }

    double outerRadius = circles.size() * 80.0;
    for(int i = 0; i < circles.size(); i++)
    {
        double angle = (2.0 * M_PI * i) / circles.size();
        QPointF anchor(qCos(angle) * outerRadius,
                       qSin(angle) * outerRadius);


        QList<MindMapNode*> &myNodes = circles[i];

        myNodes[0]->setPos(anchor);
        QSet<MindMapNode*> placed;
        placed.insert(myNodes[0]);

        QQueue<MindMapNode*> queue;
        queue.enqueue(myNodes[0]);

        while (!queue.isEmpty())
        {
            MindMapNode *current = queue.dequeue();

            QList<MindMapNode*> unplacedNeighbors;
            for (const QUuid &uuid : current->MyConnections)
            {
                MindMapNode *nb = Nodes.value(uuid);
                if (nb && !placed.contains(nb)) unplacedNeighbors.append(nb);
            }
            for (MindMapNode *nb : incomingConnections.value(current))
            {
                if (!placed.contains(nb)) unplacedNeighbors.append(nb);
            }

            int total = unplacedNeighbors.size();
            for (int i = 0; i < total; i++)
            {
                MindMapNode *nb = unplacedNeighbors[i];
                if (placed.contains(nb)) continue;

                qreal angle = (2.0 * M_PI * i) / qMax(total, 1);
                qreal spread = 80.0 + total * 15.0;
                QPointF offset(qCos(angle) * spread, qSin(angle) * spread);
                nb->setPos(current->pos() + offset);

                placed.insert(nb);
                queue.enqueue(nb);
            }
        }
    }

    simTimer = new QTimer(this);
    connect(simTimer, &QTimer::timeout, this, &MindMapView::nodeForceLayout);
    simTimer->start(16);


    setDragMode(QGraphicsView::NoDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

void MindMapView::wheelEvent(QWheelEvent *event)
{
    const double factor = 1.15;
    if (event->angleDelta().y() > 0)
    {
        if (transform().m11() < 10.0)
            scale(factor, factor);
    }
    else
    {
        if (transform().m11() > 0.1)
            scale(1.0 / factor, 1.0 / factor);
    }
    event->accept();
}

void MindMapView::nodeForceLayout()
{
    if (simStep >= SIM_STEPS) {
        simTimer->stop();
        return;
    }

    QMap<MindMapNode*, QPointF> forcesForNodes;
    for (MindMapNode *node : Nodes)
        forcesForNodes[node] = {0.0, 0.0};
    for(int i = 0; i < Uuids.count(); i++)
    {
        for(int j = i + 1; j < Uuids.count(); j++)
        {
            MindMapNode *a = Nodes[Uuids[i]];
            MindMapNode *b = Nodes[Uuids[j]];

            QPointF delta = a->pos() - b->pos();
            double dist = qMax(qSqrt(delta.x()*delta.x() + delta.y()*delta.y()), 1.0);
            QPointF force = (delta / dist) * (REPULSION / (dist * dist));

            forcesForNodes[a] += force;
            forcesForNodes[b] -= force;
        }
    }

    for (int i = 0; i < Uuids.size(); i++) {
        MindMapNode *src = Nodes.value(Uuids[i]);
        for (const QUuid &targetUuid : src->MyConnections) {
            MindMapNode *tgt = Nodes.value(targetUuid);
            if (!tgt) continue;

            QPointF delta = tgt->pos() - src->pos();
            double dist = qMax(qSqrt(delta.x()*delta.x() + delta.y()*delta.y()), 1.0);
            QPointF force = (delta / dist) * SPRING * (dist - IDEAL_LENGTH);

            forcesForNodes[src] += force;
            forcesForNodes[tgt] -= force;
        }
    }


    double damping = 1.0 - (double)simStep / SIM_STEPS;
    for (MindMapNode *node : Nodes)
        node->setPos(node->pos() + forcesForNodes[node] * damping);

    scene()->update();
    simStep++;
}
