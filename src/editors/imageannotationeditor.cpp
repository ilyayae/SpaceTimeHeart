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
