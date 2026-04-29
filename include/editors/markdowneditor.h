#ifndef MARKDOWNEDITOR_H
#define MARKDOWNEDITOR_H

#include <QMainWindow>
#include <QLabel>
#include <QTextBrowser>
#include <QSlider>
#include <QScrollBar>
#include <QTimer>
#include <QListWidget>
#include <QDialog>
#include <QDialogButtonBox>

#include "include/subclasses/customtextbrowser.h"
#include "include/subclasses/findreplacewidget.h"
#include "include/noteTypes/uuidregistry.h"

namespace Ui {
class MarkdownEditor;
}

class MarkdownEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit MarkdownEditor(QWidget *parent = nullptr, UuidRegistry *reg = nullptr);
    ~MarkdownEditor();
    CustomTextBrowser* GetQTextEdit();
    QList<int> findText();
    void selectText();
    void highlightText(bool b);
    int startZoom = 0;
    int currZoom = 0;
    UuidRegistry *registry;

public slots:
    void updateZoom(int zoom);
    void myTextEdit_scrolled(int value);
    void myTextView_scrolled(int value);
    void on_actionFind_toggled(bool arg1);
    void hyperlinkTextEdit_textChanged();

    void on_actionSave_triggered();

    void on_actionSaveAs_triggered();

    void on_actionCut_triggered();

    void on_actionCopy_triggered();

    void on_actionPaste_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionBold_triggered();

    void on_actionItalic_triggered();

    void on_actionStrikethrough_triggered();

    void on_actionUnderline_triggered();

    void on_actionDotList_triggered();

    void on_actionNumberedList_triggered();

    void on_actionMarkList_triggered();

    void on_actionHeader_triggered();

    void on_actionLine_triggered();

    void on_actionQuote_triggered();

    void on_actionCodeBlock_triggered();

    void setEditorText(QString content);

    void on_actionInsertLink_triggered();

private:
    Ui::MarkdownEditor *ui;
    CustomTextBrowser *myTextEdit = nullptr;
    CustomTextBrowser *myTextView = nullptr;
    QScrollBar *textEditScroll = nullptr;
    QScrollBar *textViewScroll = nullptr;
    FindReplaceWidget *myFRWidget = nullptr;
    QTimer *markdownUpdateTimer;
    bool syncingScroll = false;
signals:
    void saveButton();
    void saveAsButton();
    void Updated();
    void zoomChanged(int zoom);
    void uuidClicked(QUuid uuid);
};

#endif // MARKDOWNEDITOR_H
