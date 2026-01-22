#include "documenthandler.h"
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>


DocumentHandler::DocumentHandler(QWidget *parent, QSettings *settings, QGridLayout *editorPlace)
    : QWidget{parent}
{
    Settings = settings;
    EditorPlace = editorPlace;
    currentEditor = NUL;
    switchEditor(HTML);
}


void DocumentHandler::loadFile(QString fileName) {
    if (fileName.isEmpty()) return;
    QFileInfo info(fileName);
    QString suffix = info.completeSuffix();

    if (suffix == "txt")
    {
        switchEditor(TEXT);
    } else if (suffix == "md")
    {
        switchEditor(MARKDOWN);
    } else if (suffix == "html")
    {
        switchEditor(HTML);
    } else
    {
        switchEditor(EMPTY);
    }

    if (textEdit != nullptr)
    {
        saveable = false;

        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(nullptr, tr("Error"), tr("Cannot open file: %1").arg(file.errorString()));
            return;
        }

        QTextStream in(&file);
        textEdit->setPlainText(in.readAll());
        filePath = fileName;

        saveable = true;
    }

}

void DocumentHandler::saveFile() {
    if (filePath.isEmpty()) {
        filePath = QFileDialog::getSaveFileName(nullptr, tr("Save New File"), Settings->value("general/WorkDirectory", "/home").toString(), tr("Text Files (*.txt);;All Files (*)"));
        if (filePath.isEmpty()) return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, tr("Error"), tr("Cannot save file: %1").arg(file.errorString()));
        return;
    }

    QTextStream out(&file);
    out << textEdit->toPlainText();
}

void DocumentHandler::saveAsFile() {

    filePath = QFileDialog::getSaveFileName(nullptr, tr("Save File As"), Settings->value("general/WorkDirectory", "/home").toString(), tr("Text Files (*.txt);;All Files (*)"));
    if (filePath.isEmpty()) return;
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, tr("Error"), tr("Cannot save file: %1").arg(file.errorString()));
        return;
    }

    QTextStream out(&file);
    out << textEdit->toPlainText();
}

void DocumentHandler::switchEditor(CurrentEditor SwitchTo)
{
    if(textEdit != nullptr)
    {
        saveFile();
    }
    switch (currentEditor) {
    case EMPTY:
        EditorPlace->removeWidget(emptyEditor);
        emptyEditor->deleteLater();;
        break;
    case TEXT:
        EditorPlace->removeWidget(textEditor);
        textEditor->deleteLater();;
        break;
    case MARKDOWN:
        EditorPlace->removeWidget(markdownEditor);
        markdownEditor->deleteLater();;
        break;
    case HTML:
        EditorPlace->removeWidget(htmlEditor);
        htmlEditor->deleteLater();;
        break;
    default:
        break;
    }

    switch (SwitchTo) {
    case EMPTY:
        emptyEditor = new EmptyEditor();
        EditorPlace->addWidget(emptyEditor);
        textEdit = nullptr;
        break;
    case TEXT:
        textEditor = new TextEditor();
        EditorPlace->addWidget(textEditor);
        textEdit = textEditor->getQTextEdit();
        connect(textEditor, &TextEditor::saveButton, this, &DocumentHandler::saveFile);
        connect(textEditor, &TextEditor::saveAsButton, this, &DocumentHandler::saveAsFile);
        break;
    case MARKDOWN:

        break;
    case HTML:
        htmlEditor = new HtmlEditor();
        EditorPlace->addWidget(htmlEditor);
        textEdit = nullptr;

        break;
    default:
        break;
    }
    currentEditor = SwitchTo;
}
