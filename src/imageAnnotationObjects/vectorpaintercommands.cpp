#include "include/imageAnnotationObjects/vectorpaintercommands.h"

VectorPainterCommands::VectorPainterCommands() {}

AddPointCommand::AddPointCommand(QList<QPair<double, double>> *shapesPoints, QPair<double, double> addPoint)
    : ShapesPoints(shapesPoints), AddPoint(addPoint)
{
    //Nothing
}
void AddPointCommand::redo()
{
    ShapesPoints->append(AddPoint);
}
void AddPointCommand::undo()
{
    ShapesPoints->removeAt(ShapesPoints->lastIndexOf(AddPoint));
}


FinalizeShapeCommand::FinalizeShapeCommand(QList<QPair<double, double>> *shapesPoints, bool closed, LineStyle *styleOfLine, FillStyle *styleOfFill, ImageAnnotationData *data)
    : Data(data)
{
    Shape = new ShapeData();
    Shape->Closed = closed;
    Shape->StyleOfFill = *styleOfFill;
    Shape->StyleOfLine = *styleOfLine;
    Shape->XYPoints = *shapesPoints;
}
FinalizeShapeCommand::~FinalizeShapeCommand()
{
    delete Shape;
}
void FinalizeShapeCommand::redo()
{
    Data->shapes.append(*Shape);
}
void FinalizeShapeCommand::undo()
{
    Data->shapes.removeAt(Data->shapes.lastIndexOf(*Shape));
}


MovePointCommand::MovePointCommand(ShapeGraphicsObject *shape, int id, QPair<double, double> newLocation)
    :  NewLocation(newLocation), Shape(shape), Id(id)
{
    OldLocation = Shape->MyData->XYPoints[Id];
}
void MovePointCommand::redo()
{
    Shape->MyData->XYPoints[Id] = NewLocation;
    Shape->syncFromData();
}
void MovePointCommand::undo()
{
    Shape->MyData->XYPoints[Id] = OldLocation;
    Shape->syncFromData();
}


DeleteShapeCommand::DeleteShapeCommand(ImageAnnotationData *data, ShapeData shape)
    : Data(data), Shape(shape)
{
    Id = Data->shapes.lastIndexOf(Shape);
}
void DeleteShapeCommand::redo()
{
    Data->shapes.removeAt(Id);
}
void DeleteShapeCommand::undo()
{
    Data->shapes.insert(Id, Shape);
}


ChangeStyleCommand::ChangeStyleCommand(ShapeData *shape, ShapeData newData)
    : Shape(shape), NewData(newData)
{
    OldData = *Shape;
}
void ChangeStyleCommand::redo()
{
    *Shape = NewData;
}
void ChangeStyleCommand::undo()
{
    *Shape = OldData;
}
