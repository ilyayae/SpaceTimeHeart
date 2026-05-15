#include "include/listEntries/leapdayexceptionentry.h"
#include "ui_leapdayexceptionentry.h"

LeapDayExceptionEntry::LeapDayExceptionEntry(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LeapDayExceptionEntry)
{
    ui->setupUi(this);
}

LeapDayExceptionEntry::~LeapDayExceptionEntry()
{
    delete ui;
}

void LeapDayExceptionEntry::setMe(bool hap, int prio, int NYears, int offset)
{
    happens = hap;
    priority = prio;
    everyNYears = NYears;
    yearOffset = offset;
    ui->checkBoxHappens->setChecked(hap);
    ui->spinBoxPriority->setValue(prio);
    ui->spinBoxEveryNYears->setValue(NYears);
    ui->spinBoxYearsOffset->setValue(offset);
}

void LeapDayExceptionEntry::on_checkBoxHappens_checkStateChanged(const Qt::CheckState &arg1)
{
    happens = arg1;
}

void LeapDayExceptionEntry::on_spinBoxPriority_valueChanged(int arg1)
{
    priority = arg1;
}

void LeapDayExceptionEntry::on_spinBoxEveryNYears_valueChanged(int arg1)
{
    everyNYears = arg1;
}

void LeapDayExceptionEntry::on_spinBoxYearsOffset_valueChanged(int arg1)
{
    yearOffset = arg1;
}


void LeapDayExceptionEntry::on_RemoveButton_clicked()
{
    emit destroyMe(this);
}

