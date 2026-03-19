#include "include/listEntries/moonentry.h"
#include "ui_moonentry.h"

MoonEntry::MoonEntry(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MoonEntry)
{
    ui->setupUi(this);
    colorHex = "#C0C0C0";
    ui->LineCycle->setValidator(new QDoubleValidator(0.0, 9999.0, 4, this));
    ui->LineOffset->setValidator(new QDoubleValidator(0.0, 9999.0, 4, this));
    ui->ColorButton->setStyleSheet("background-color: " + colorHex + ";");
}

MoonEntry::~MoonEntry()
{
    delete ui;
}

void MoonEntry::on_RemoveButton_clicked()
{
    emit destroyMe(this);
}


void MoonEntry::on_LineName_textChanged(const QString &arg1)
{
    name = arg1;
}


void MoonEntry::on_LineCycle_textChanged(const QString &arg1)
{
    cycle = arg1.toFloat();
}


void MoonEntry::on_LineOffset_textChanged(const QString &arg1)
{
    offset = arg1.toFloat();
}


void MoonEntry::on_ColorButton_clicked()
{
    QColor c = QColorDialog::getColor(QColor::fromString(colorHex), this, "Pick a color");
    if (c.isValid()) {
        colorHex = c.name();
        ui->ColorButton->setStyleSheet("background-color: " + c.name() + ";");
    }
}

