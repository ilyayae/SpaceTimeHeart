#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QMainWindow>
#include <QTextBrowser>
#include <QClipboard>
#include <QSlider>
#include <QLabel>

#include "include/subclasses/hyperlinktextbrowser.h"
#include "include/subclasses/findreplacewidget.h"

namespace Ui {
class TextEditor;
}

class TextEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit TextEditor(QWidget *parent = nullptr);
    ~TextEditor();
    QTextBrowser* getQTextEdit();
    void highlightText(bool b);

private slots:
    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionSave_triggered();

    void on_actionSaveAs_triggered();

    void on_actionCut_triggered();

    void on_actionCopy_triggered();

    void on_actionPaste_triggered();

    void on_actionFind_toggled(bool arg1);

    void updateZoom(int zoom);

    void hyperlinkTextEdit_textChanged();

private:
    Ui::TextEditor *ui;
    HyperlinkTextBrowser *myTextEdit = nullptr;
    FindReplaceWidget *myFRWidget = nullptr;
    QList<int> textFoundPositions;
    int currentFoundPosition;
    int currentFoundWordLength;
    bool isActivelySearchingWords;
    int currZoom;
signals:
    void saveButton();
    void saveAsButton();
};
#endif // TEXTEDITOR_H
