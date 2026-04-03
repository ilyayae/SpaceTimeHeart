#ifndef VECTORPAINTERCOMMANDS_H
#define VECTORPAINTERCOMMANDS_H

#include "include/noteTypes/imageannotationdata.h"
#include "include/imageAnnotationObjects/shapegraphicsobject.h"
#include <QUndoCommand>

class VectorPainterCommands : public QUndoCommand
{
public:
    VectorPainterCommands();
};

class AddPointCommand : public QUndoCommand
{
public:
    AddPointCommand(QList<QPair<double, double>> *shapesPoints, QPair<double, double> addPoint);
    void redo() override;
    void undo() override;
private:
    QList<QPair<double, double>> *ShapesPoints;
    QPair<double, double> AddPoint;
};

class FinalizeShapeCommand : public QUndoCommand
{
public:
    FinalizeShapeCommand(QList<QPair<double, double>> *shapesPoints, bool closed, LineStyle *styleOfLine, FillStyle *styleOfFill, ImageAnnotationData *data);
    ~FinalizeShapeCommand() override;
    void redo() override;
    void undo() override;
private:
    ImageAnnotationData *Data;
    ShapeData *Shape;
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
