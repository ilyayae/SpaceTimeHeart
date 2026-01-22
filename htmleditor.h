#ifndef HTMLEDITOR_H
#define HTMLEDITOR_H

#include <QMainWindow>

namespace Ui {
class HtmlEditor;
}

class HtmlEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit HtmlEditor(QWidget *parent = nullptr);
    ~HtmlEditor();

private:
    Ui::HtmlEditor *ui;
};

#endif // HTMLEDITOR_H
