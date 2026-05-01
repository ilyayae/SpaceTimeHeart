#include "include/listEntries/dayinweekentry.h"
#include "ui_dayinweekentry.h"

DayInWeekEntry::DayInWeekEntry(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DayInWeekEntry)
{
    ui->setupUi(this);
    ui->LineName->setText("Weekday");
}

DayInWeekEntry::~DayInWeekEntry()
{
    delete ui;
}

void DayInWeekEntry::on_pushButton_clicked()
{
    emit destroyMe(this);
}

void DayInWeekEntry::setMe(QString Name)
{
    name = Name;
    ui->LineName->setText(name);
}

void DayInWeekEntry::on_LineName_textChanged(const QString &arg1)
{
    name = arg1;
}

