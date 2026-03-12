#include "include/editors/calendareditor.h"
#include "ui_calendareditor.h"

CalendarEditor::CalendarEditor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CalendarEditor)
{
    ui->setupUi(this);
}

CalendarEditor::~CalendarEditor()
{
    delete ui;
}
