#ifndef HTMLEDITOR_H
#define HTMLEDITOR_H

#include <QWidget>

namespace Ui {
class HtmlEditor;
}

class HtmlEditor : public QWidget
{
    Q_OBJECT

public:
    explicit HtmlEditor(QWidget *parent = nullptr);
    ~HtmlEditor();

private:
    Ui::HtmlEditor *ui;
};

#endif // HTMLEDITOR_H
