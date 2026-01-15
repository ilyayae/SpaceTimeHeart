#include "htmleditor.h"
#include "ui_htmleditor.h"

HtmlEditor::HtmlEditor(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HtmlEditor)
{
    ui->setupUi(this);
}

HtmlEditor::~HtmlEditor()
{
    delete ui;
}
