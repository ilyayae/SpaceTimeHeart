#include "newfiledialog.h"
#include "ui_newfiledialog.h"

newFileDialog::newFileDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::newFileDialog)
{
    ui->setupUi(this);
}

newFileDialog::~newFileDialog()
{
    delete ui;
}
