#include "documenthandler.h"
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

DocumentHandler::DocumentHandler(QWidget *parent, QSettings *settings)
    : QWidget{parent}
{
    setts = settings;
}

void DocumentHandler::loadFile(QTextEdit *editor, QString fileName) {
    if (fileName.isEmpty()) return;

    saveable = false;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, tr("Error"), tr("Cannot open file: %1").arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    editor->setPlainText(in.readAll());
    filePath = fileName;

    saveable = true;
}

void DocumentHandler::saveFile(QTextEdit *editor) {
    if (filePath.isEmpty()) {
        filePath = QFileDialog::getSaveFileName(nullptr, tr("Save New File"), setts->value("general/WorkDirectory", "/home").toString(), tr("Text Files (*.txt);;All Files (*)"));
        if (filePath.isEmpty()) return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, tr("Error"), tr("Cannot save file: %1").arg(file.errorString()));
        return;
    }

    QTextStream out(&file);
    out << editor->toPlainText();
}
