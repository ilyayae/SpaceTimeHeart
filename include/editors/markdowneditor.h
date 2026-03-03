#ifndef MARKDOWNEDITOR_H
#define MARKDOWNEDITOR_H

#include <QMainWindow>
#include <QLabel>
#include <QTextBrowser>
#include <QSlider>
#include <QScrollBar>

#include "include/subclasses/hyperlinktextbrowser.h"
#include "include/subclasses/findreplacewidget.h"
#include "include/subclasses/searchhighlighter.h"

namespace Ui {
class MarkdownEditor;
}

class MarkdownEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit MarkdownEditor(QWidget *parent = nullptr);
    ~MarkdownEditor();
    QTextBrowser* GetQTextEdit();
    QList<int> findText();
    void selectText();
    void highlightText(bool b);

private slots:
    void updateZoom(int zoom);
    void myTextEdit_scrolled(int value);
    void myTextView_scrolled(int value);
    void on_actionFind_toggled(bool arg1);
    void hyperlinkTextEdit_textChanged();

private:
    Ui::MarkdownEditor *ui;
    HyperlinkTextBrowser *myTextEdit = nullptr;
    QTextBrowser *myTextView = nullptr;
    QScrollBar *textEditScroll = nullptr;
    QScrollBar *textViewScroll = nullptr;
    FindReplaceWidget *myFRWidget = nullptr;
    int currZoom;
    bool syncingScroll = false;
};

#endif // MARKDOWNEDITOR_H
