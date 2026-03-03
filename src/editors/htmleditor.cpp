#include "include/editors/htmleditor.h"
#include "ui_htmleditor.h"

HtmlEditor::HtmlEditor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HtmlEditor)
{
    ui->setupUi(this);
}

HtmlEditor::~HtmlEditor()
{
    delete ui;
}
