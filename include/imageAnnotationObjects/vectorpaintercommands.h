#ifndef VECTORPAINTERCOMMANDS_H
#define VECTORPAINTERCOMMANDS_H

#include "include/imageAnnotationObjects/shapeinprogress.h"
#include "include/noteTypes/imageannotationdata.h"
#include "include/imageAnnotationObjects/shapegraphicsobject.h"
#include <QUndoCommand>

class VectorPainterCommands : public QUndoCommand
{
public:
    VectorPainterCommands();
};
class StartPainting : public QUndoCommand
{
public:
    StartPainting(ShapeInProgress **shape, QPair<double, double> startPoint, QColor *currentLineColor, QColor *currentFillColor, Qt::PenStyle *currentPenStyle,
                  Qt::BrushStyle *currentBrushStyle, int *currentRounding, int *currentWidth, QGraphicsPixmapItem *image, QGraphicsScene *scene);
    void redo() override;
    void undo() override;
private:
    ShapeInProgress **Shape;
    QPair<double, double> StartPoint;
    QColor *storedLineColor;
    QColor *storedFillColor;
    Qt::PenStyle *storedPenStyle;
    Qt::BrushStyle *storedBrushStyle;
    int *storedRounding;
    int *storedWidth;
    QGraphicsPixmapItem *storedImage;
    QGraphicsScene *Scene;
};

class AddPointCommand : public QUndoCommand
{
public:
    AddPointCommand(ShapeInProgress **shape, QPair<double, double> addPoint);
    void redo() override;
    void undo() override;
private:
    ShapeInProgress **Shape;
    QPair<double, double> AddPoint;
};

class FinalizeShapeCommand : public QUndoCommand
{
public:
    FinalizeShapeCommand(ShapeInProgress **shape, bool closed, LineStyle styleOfLine, FillStyle styleOfFill, int rounding, ImageAnnotationData *data, QPair<double, double> newLocation);
    ~FinalizeShapeCommand() override;
    void redo() override;
    void undo() override;
private:
    ShapeInProgress **Shape;
    QPair<double, double> NewLocation;
    ShapeData *newShape;
    ImageAnnotationData *Data;
    QList<QPair<double, double>> storedPoints;
    QColor *storedLineColor;
    QColor *storedFillColor;
    Qt::PenStyle *storedPenStyle;
    Qt::BrushStyle *storedBrushStyle;
    int *storedRounding;
    int *storedWidth;
    QGraphicsPixmapItem *storedImage;
    QGraphicsScene *Scene;
};

class MovePointCommand : public QUndoCommand
{
public:
    MovePointCommand(ShapeGraphicsObject *shape, int id, QPair<double, double> newLocation);
    void redo() override;
    void undo() override;
private:
    QPair<double, double> NewLocation;
    QPair<double, double> OldLocation;
    ShapeGraphicsObject *Shape;
    int Id;
};

class DeleteShapeCommand : public QUndoCommand
{
public:
    DeleteShapeCommand(ImageAnnotationData *data, ShapeData shape);
    void redo() override;
    void undo() override;
private:
    ImageAnnotationData *Data;
    ShapeData Shape;
    int Id;
};

class ChangeStyleCommand : public QUndoCommand
{
public:
    ChangeStyleCommand(ShapeData *shape, ShapeData newData);
    void redo() override;
    void undo() override;
private:
    ShapeData *Shape;
    ShapeData OldData;
    ShapeData NewData;
};



#endif // VECTORPAINTERCOMMANDS_H
