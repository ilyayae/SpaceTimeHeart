#include "include/imageAnnotationObjects/vectorpaintercommands.h"

VectorPainterCommands::VectorPainterCommands() {}
StartPainting::StartPainting(ShapeInProgress **shape, QPair<double, double> startPoint, QColor *currentLineColor, QColor *currentFillColor, Qt::PenStyle *currentPenStyle,
                             Qt::BrushStyle *currentBrushStyle, int *currentRounding, int *currentWidth, QGraphicsPixmapItem *image, QGraphicsScene *scene)
    : Shape(shape), StartPoint(startPoint), storedLineColor(currentLineColor), storedFillColor(currentFillColor), storedPenStyle(currentPenStyle),
    storedBrushStyle(currentBrushStyle), storedRounding(currentRounding), storedWidth(currentWidth), storedImage(image), Scene(scene)
{
}

void StartPainting::redo()
{
    *Shape = new ShapeInProgress(StartPoint, storedLineColor, storedFillColor, storedPenStyle, storedBrushStyle, storedRounding, storedWidth, storedImage);
    Scene->addItem(*Shape);
}

void StartPainting::undo()
{
    Scene->removeItem(*Shape);
    delete *Shape;
    *Shape = nullptr;
}

AddPointCommand::AddPointCommand(ShapeInProgress **shape, QPair<double, double> addPoint)
    : Shape(shape), AddPoint(addPoint)
{
}

void AddPointCommand::redo()
{
    (*Shape)->XYPoints.append(AddPoint);
    (*Shape)->update();
}

void AddPointCommand::undo()
{
    (*Shape)->XYPoints.removeAt((*Shape)->XYPoints.lastIndexOf(AddPoint));
    (*Shape)->update();
}

FinalizeShapeCommand::FinalizeShapeCommand(ShapeInProgress **shape, bool closed, LineStyle styleOfLine, FillStyle styleOfFill, int rounding, ImageAnnotationData *data, QPair<double, double> newLocation)
    : Shape(shape), NewLocation(newLocation), Data(data)
{
    newShape = new ShapeData();
    newShape->Closed = closed;
    newShape->StyleOfFill = styleOfFill;
    newShape->StyleOfLine = styleOfLine;
    newShape->XYPoints = (*shape)->XYPoints;
    if(!newShape->Closed)
        newShape->XYPoints.append(NewLocation);
    newShape->rounding = rounding;

    storedPoints = (*shape)->XYPoints;
    storedLineColor = (*shape)->CurrentLineColor;
    storedFillColor = (*shape)->CurrentFillColor;
    storedPenStyle = (*shape)->CurrentPenStyle;
    storedBrushStyle = (*shape)->CurrentBrushStyle;
    storedRounding = (*shape)->CurrentRounding;
    storedWidth = (*shape)->CurrentWidth;
    storedImage = (*shape)->Image;
    Scene = (*shape)->scene();
}

FinalizeShapeCommand::~FinalizeShapeCommand()
{
    delete newShape;
}

void FinalizeShapeCommand::redo()
{
    Scene->removeItem(*Shape);
    delete *Shape;
    *Shape = nullptr;
    Data->shapes.append(*newShape);
}

void FinalizeShapeCommand::undo()
{
    Data->shapes.removeAt(Data->shapes.lastIndexOf(*newShape));
    *Shape = new ShapeInProgress(storedPoints.first(), storedLineColor, storedFillColor, storedPenStyle, storedBrushStyle, storedRounding, storedWidth, storedImage);
    (*Shape)->XYPoints = storedPoints;
    Scene->addItem(*Shape);
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
