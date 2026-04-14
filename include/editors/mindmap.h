#ifndef MINDMAP_H
#define MINDMAP_H

#include <QMainWindow>
#include <QLayout>
#include <include/noteTypes/uuidregistry.h>
#include <include/mindMapObjects/mindmapview.h>

namespace Ui {
class MindMap;
}

class MindMap : public QMainWindow
{
    Q_OBJECT

public:
    explicit MindMap(QWidget *parent = nullptr, UuidRegistry *reg = nullptr);
    ~MindMap();
    UuidRegistry *myRegistry;
    void extracted();
    void setUp();

private:
    Ui::MindMap *ui;
    MindMapView *myView;
};

#endif // MINDMAP_H
