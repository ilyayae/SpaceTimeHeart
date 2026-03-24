#include "include/editors/imageannotationeditor.h"
#include "ui_imageannotationeditor.h"

ImageAnnotationEditor::ImageAnnotationEditor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ImageAnnotationEditor)
{
    ui->setupUi(this);
}

ImageAnnotationEditor::~ImageAnnotationEditor()
{
    delete ui;
}

void ImageAnnotationEditor::Initialize(ImageAnnotationData *data)
{
    myData = data;
    QPixmap *pixMap = new QPixmap();
    pixMap->loadFromData(myData->imageData);
    QGraphicsPixmapItem *imageItem = new QGraphicsPixmapItem();
    imageItem->setPixmap(*pixMap);
    CustomGraphicsView *graphicsView = new CustomGraphicsView(ui->centralwidget);
    ui->centralwidget->layout()->addWidget(graphicsView);
    graphicsView->scene()->addItem(imageItem);
}
