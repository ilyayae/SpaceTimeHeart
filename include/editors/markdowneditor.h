#ifndef MARKDOWNEDITOR_H
#define MARKDOWNEDITOR_H

#include <QMainWindow>
#include <QLabel>
#include <QTextBrowser>
#include <QSlider>

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

    void on_actionFind_toggled(bool arg1);

private:
    Ui::MarkdownEditor *ui;
    HyperlinkTextBrowser *myTextEdit = nullptr;
    FindReplaceWidget *myFRWidget = nullptr;
    int currZoom;
};

#endif // MARKDOWNEDITOR_H
