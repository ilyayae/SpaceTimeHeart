#include "texteditor.h"
#include "ui_texteditor.h"

TextEditor::TextEditor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TextEditor)
{
    ui->setupUi(this);
}

TextEditor::~TextEditor()
{
    delete ui;
}

QTextEdit* TextEditor::getQTextEdit()
{
    ui->FindReplaceWidget->setVisible(false);
    return ui->textEdit;
}

void TextEditor::on_actionUndo_triggered()
{
    ui->textEdit->undo();
}


void TextEditor::on_actionRedo_triggered()
{
    ui->textEdit->redo();
}


void TextEditor::on_actionSave_triggered()
{
    emit saveButton();
}


void TextEditor::on_actionSaveAs_triggered()
{
    emit saveAsButton();
}


void TextEditor::on_actionCut_triggered()
{
    ui->textEdit->cut();
}


void TextEditor::on_actionCopy_triggered()
{
    ui->textEdit->copy();
}


void TextEditor::on_actionPaste_triggered()
{
    ui->textEdit->paste();
}


void TextEditor::on_actionFind_triggered()
{

}


void TextEditor::on_actionFind_toggled(bool checked)
{
    ui->FindReplaceWidget->setVisible(checked);
}

