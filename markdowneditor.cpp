#include "markdowneditor.h"
#include "ui_markdowneditor.h"

MarkdownEditor::MarkdownEditor(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MarkdownEditor)
{
    ui->setupUi(this);
}

MarkdownEditor::~MarkdownEditor()
{
    delete ui;
}
