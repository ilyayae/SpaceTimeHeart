#include "include/editors/emptyeditor.h"
#include "ui_emptyeditor.h"

EmptyEditor::EmptyEditor(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EmptyEditor)
{
    ui->setupUi(this);
}

EmptyEditor::~EmptyEditor()
{
    delete ui;
}
