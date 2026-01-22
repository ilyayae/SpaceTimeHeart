#ifndef MARKDOWNEDITOR_H
#define MARKDOWNEDITOR_H

#include <QMainWindow>

namespace Ui {
class MarkdownEditor;
}

class MarkdownEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit MarkdownEditor(QWidget *parent = nullptr);
    ~MarkdownEditor();

private:
    Ui::MarkdownEditor *ui;
};

#endif // MARKDOWNEDITOR_H
