#ifndef IMAGEANNOTATIONEDITOR_H
#define IMAGEANNOTATIONEDITOR_H

#include <QMainWindow>
#include <QGraphicsPixmapItem>

#include <include/noteTypes/imageannotationdata.h>
#include <include/noteTypes/uuidregistry.h>
#include <include/subclasses/customgraphicsview.h>

namespace Ui {
class ImageAnnotationEditor;
}

class ImageAnnotationEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit ImageAnnotationEditor(QWidget *parent = nullptr);
    ~ImageAnnotationEditor();
    void Initialize(ImageAnnotationData *data);
    ImageAnnotationData *myData;
    UuidRegistry *myRegistry;

private:
    Ui::ImageAnnotationEditor *ui;
};

#endif // IMAGEANNOTATIONEDITOR_H
