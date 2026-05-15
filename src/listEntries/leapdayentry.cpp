#include "include/listEntries/leapdayentry.h"
#include "include/noteTypes/calendardata.h"
#include "ui_leapdayentry.h"

LeapDayEntry::LeapDayEntry(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LeapDayEntry)
{
    ui->setupUi(this);
}

LeapDayEntry::~LeapDayEntry()
{
    delete ui;
}

void LeapDayEntry::setMonthSelection(QList<MonthInYearEntry*> listOfMonths)
{
    ui->MonthSelection->clear();
    for(MonthInYearEntry *month : listOfMonths)
    {
        ui->MonthSelection->addItem( month->name );
    }

}

void LeapDayEntry::setMe(int month, int NYears, int offset, int addDays, bool affectWeekDays, bool randomDay, bool randomMonth, QList<LeapDayException> exception)
{
    foreach (LeapDayException except, exception) {
        on_AddException_clicked();
        exceptions.last()->setMe(except.happens, except.priority, except.everyNYears, except.offsetYears);
    }
    ui->MonthSelection->setCurrentIndex(month);
    ui->EveryNYear->setValue(NYears);
    ui->Offset->setValue(offset);
    ui->DaysAdded->setValue(addDays);
    ui->AffectsWeekDay->setChecked(affectWeekDays);
    ui->RandomizeDay->setChecked(randomDay);
    ui->RandomizeMonth->setChecked(randomMonth);
    selectedMonth = month;
    everyNYears = NYears;
    offsetYears = offset;
    daysAdded = addDays;
    affectsWeekDays = affectWeekDays;
    randomizeDay = randomDay;
    randomizeMonth = randomMonth;
}

void LeapDayEntry::on_MonthSelection_activated(int index)
{
    selectedMonth = index;
}


void LeapDayEntry::on_EveryNYear_valueChanged(int arg1)
{
    everyNYears = arg1;
}


void LeapDayEntry::on_Offset_valueChanged(int arg1)
{
    offsetYears = arg1;
}


void LeapDayEntry::on_DaysAdded_valueChanged(int arg1)
{
    daysAdded = arg1;
}


void LeapDayEntry::on_AffectsWeekDay_checkStateChanged(const Qt::CheckState &arg1)
{
    affectsWeekDays = arg1;
}


void LeapDayEntry::on_RandomizeDay_checkStateChanged(const Qt::CheckState &arg1)
{
    randomizeDay = arg1;
}


void LeapDayEntry::on_RandomizeMonth_checkStateChanged(const Qt::CheckState &arg1)
{
    randomizeMonth = arg1;
}


void LeapDayEntry::on_AddException_clicked()
{
    LeapDayExceptionEntry *leap = new LeapDayExceptionEntry(ui->ListOfException);
    qobject_cast<QVBoxLayout*>(ui->ListOfException->layout())->insertWidget(exceptions.count(), leap);
    connect(leap, &LeapDayExceptionEntry::destroyMe, this, &LeapDayEntry::DestroyLeap);
    exceptions.append(leap);
}

void LeapDayEntry::on_DeleteLeapDay_clicked()
{
    emit destroyMe(this);
}

void LeapDayEntry::DestroyLeap(LeapDayExceptionEntry *leap)
{
    if(exceptions.count() > 0)
    {
        exceptions.removeOne(leap);
        leap->deleteLater();
    }
}
