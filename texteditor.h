#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QMainWindow>
#include <QTextEdit>
#include <QClipboard>

namespace Ui {
class TextEditor;
}

class TextEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit TextEditor(QWidget *parent = nullptr);
    ~TextEditor();
    QTextEdit* getQTextEdit();

private slots:
    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionSave_triggered();

    void on_actionSaveAs_triggered();

    void on_actionCut_triggered();

    void on_actionCopy_triggered();

    void on_actionPaste_triggered();

    void on_actionFind_triggered();

    void on_actionFind_toggled(bool arg1);

private:
    Ui::TextEditor *ui;
signals:
    void saveButton();
    void saveAsButton();
};
#endif // TEXTEDITOR_H
