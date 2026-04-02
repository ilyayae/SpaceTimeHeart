#ifndef IMAGEANNOTATIONEDITOR_H
#define IMAGEANNOTATIONEDITOR_H

#include <QMainWindow>
#include <QGraphicsPixmapItem>
#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QColorDialog>
#include <QListWidget>
#include <QFileInfo>
#include <QPushButton>
#include <QButtonGroup>
#include <QUndoStack>
#include <QToolButton>
#include <QCheckBox>

#include <include/noteTypes/imageannotationdata.h>
#include <include/noteTypes/uuidregistry.h>
#include <include/subclasses/customgraphicsview.h>
#include "include/imageAnnotationObjects/shapegraphicsobject.h"
#include "include/subclasses/markeritem.h"

namespace Ui {
class ImageAnnotationEditor;
}

class ImageAnnotationEditor : public QMainWindow
{
    Q_OBJECT

public:
    // Core
    explicit ImageAnnotationEditor(QWidget *parent = nullptr);
    ~ImageAnnotationEditor();
    void Initialize(ImageAnnotationData *data);
    ImageAnnotationData *myData;
    UuidRegistry *myRegistry;
    CustomGraphicsView *graphicsView = nullptr;
    QGraphicsPixmapItem *imageItem = nullptr;

    // Bool switches
    bool isEditingMarkers = false;
    bool isEditingShapes = false;

    // Marker editing
    void UpdateMarkers();
    MarkerData CreateMarker(double x, double y, MarkerData *pregenData = nullptr);
    QList<MarkerItem*> *myMarkers = new QList<MarkerItem*>();
    QList<MarkerItem*> *selectedMarkers = new QList<MarkerItem*>();

    // Shapes painting/editing
    void UpdateShapes();
    //ShapeData CreateShape(ShapeData *pregenData = nullptr);
    QList<ShapeGraphicsObject*> *myShapes = new QList<ShapeGraphicsObject*>();
    QUndoStack undoStack;
    QColor currentLineColor = QColor(0, 0, 0, 127);
    QColor currentFillColor = QColor(0, 0, 0, 63);
    Qt::PenStyle currentPenStyle = Qt::SolidLine;
    Qt::BrushStyle currentBrushStyle = Qt::SolidPattern;
    int currentRounding = 0;

private slots:
    void on_actionShapesEditMode_toggled(bool arg1);
    void on_actionMoveMarkers_toggled(bool arg1);
    void on_actionAddMarker_triggered();
    void emitUuid(QString uuid);
    void ClickedMarker(MarkerItem *mark, bool shift);
    void RightClickedMarker(MarkerItem *mark);
    void RightClickedShape(ShapeGraphicsObject *shape);
    void dragMarkers(QPointF delta);
    void finishDragging(QPointF delta);
    void on_actionRemoveMarker_triggered();

signals:
    void Updated();
    void uuidClicked(QUuid uuid);

private:
    Ui::ImageAnnotationEditor *ui;
};

#endif // IMAGEANNOTATIONEDITOR_H
