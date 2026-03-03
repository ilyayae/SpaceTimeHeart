#ifndef EMPTYEDITOR_H
#define EMPTYEDITOR_H

#include <QWidget>

namespace Ui {
class EmptyEditor;
}

class EmptyEditor : public QWidget
{
    Q_OBJECT

public:
    explicit EmptyEditor(QWidget *parent = nullptr);
    ~EmptyEditor();

private:
    Ui::EmptyEditor *ui;
};

#endif // EMPTYEDITOR_H
