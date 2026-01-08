#ifndef NEWFILEDIALOG_H
#define NEWFILEDIALOG_H

#include <QDialog>

namespace Ui {
class newFileDialog;
}

class newFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit newFileDialog(QWidget *parent = nullptr);
    ~newFileDialog();

private:
    Ui::newFileDialog *ui;
};

#endif // NEWFILEDIALOG_H
