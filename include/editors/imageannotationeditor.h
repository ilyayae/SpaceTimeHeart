#ifndef IMAGEANNOTATIONEDITOR_H
#define IMAGEANNOTATIONEDITOR_H

#include <QMainWindow>

namespace Ui {
class ImageAnnotationEditor;
}

class ImageAnnotationEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit ImageAnnotationEditor(QWidget *parent = nullptr);
    ~ImageAnnotationEditor();

private:
    Ui::ImageAnnotationEditor *ui;
};

#endif // IMAGEANNOTATIONEDITOR_H
