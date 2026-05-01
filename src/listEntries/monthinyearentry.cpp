#include "include/listEntries/monthinyearentry.h"
#include "ui_monthinyearentry.h"

MonthInYearEntry::MonthInYearEntry(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MonthInYearEntry)
{
    ui->setupUi(this);
    ui->LineDays->setValidator(new QIntValidator(0, 9999, this));
    ui->LineName->setText("Month");
    ui->LineDays->setText("30");
    name = "Month";
    days = 30;
}

MonthInYearEntry::~MonthInYearEntry()
{
    delete ui;
}

void MonthInYearEntry::on_pushButton_clicked()
{
    emit destroyMe(this);
}

void MonthInYearEntry::setMe(QString Name, int Days)
{
    name = Name;
    days = Days;
    ui->LineName->setText(name);
    ui->LineDays->setText(QString::number(days));
}

void MonthInYearEntry::on_LineDays_textChanged(const QString &arg1)
{
    days = arg1.toInt();
}


void MonthInYearEntry::on_LineName_textChanged(const QString &arg1)
{
    name = arg1;
}

