#include "include/listEntries/moonentry.h"
#include "ui_moonentry.h"

MoonEntry::MoonEntry(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MoonEntry)
{
    ui->setupUi(this);
}

MoonEntry::~MoonEntry()
{
    delete ui;

    ui->LineCycle->setValidator(new QDoubleValidator(0.0, 9999.0, 4, this));
    ui->LineOffset->setValidator(new QDoubleValidator(0.0, 9999.0, 4, this));
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

