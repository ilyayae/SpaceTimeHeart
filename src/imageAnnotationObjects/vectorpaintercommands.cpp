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


MovePointCommand::MovePointCommand(ShapeData *shape, int id, QPair<double, double> newLocation)
    :  NewLocation(newLocation), Shape(shape), Id(id)
{
    OldLocation = Shape->XYPoints[Id];
}
void MovePointCommand::redo()
{
    Shape->XYPoints[Id] = NewLocation;
}
void MovePointCommand::undo()
{
    Shape->XYPoints[Id] = OldLocation;
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


ChangeStyleCommand::ChangeStyleCommand(ShapeData *shape, LineStyle styleOfLineNEW, FillStyle styleOfFillNEW)
    : Shape(shape), StyleOfLineNEW(styleOfLineNEW), StyleOfFillNEW(styleOfFillNEW)
{
    StyleOfLineOLD = Shape->StyleOfLine;
    StyleOfFillOLD = Shape->StyleOfFill;
}
void ChangeStyleCommand::redo()
{
    Shape->StyleOfLine = StyleOfLineNEW;
    Shape->StyleOfFill = StyleOfFillNEW;
}
void ChangeStyleCommand::undo()
{
    Shape->StyleOfLine = StyleOfLineOLD;
    Shape->StyleOfFill = StyleOfFillOLD;
}
