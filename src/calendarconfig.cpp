#include "include/calendarconfig.h"
#include "ui_calendarconfig.h"

CalendarConfig::CalendarConfig(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CalendarConfig)
{
    ui->setupUi(this);
}

CalendarConfig::~CalendarConfig()
{
    delete ui;
}

void CalendarConfig::on_AddMonth_clicked()
{
    MonthInYearEntry *month = new MonthInYearEntry(ui->MonthsInYearEdit);
    qobject_cast<QVBoxLayout*>(ui->MonthsInYearEdit->layout())->insertWidget(listMonth.count(), month);
    connect(month, &MonthInYearEntry::destroyMe, this, &CalendarConfig::DestroyMonth);
    listMonth.append(month);
}


void CalendarConfig::on_AddMoon_clicked()
{
    MoonEntry *moon = new MoonEntry(ui->MoonsAndPhasesEdit);
    qobject_cast<QVBoxLayout*>(ui->MoonsAndPhasesEdit->layout())->insertWidget(listMoon.count(), moon);
    connect(moon, &MoonEntry::destroyMe, this, &CalendarConfig::DestroyMoon);
    listMoon.append(moon);
}


void CalendarConfig::on_AddDay_clicked()
{
    DayInWeekEntry *day = new DayInWeekEntry(ui->DaysInWeekEdit);
    qobject_cast<QVBoxLayout*>(ui->DaysInWeekEdit->layout())->insertWidget(listDay.count(), day);
    connect(day, &DayInWeekEntry::destroyMe, this, &CalendarConfig::DestroyDay);
    listDay.append(day);
}

void CalendarConfig::DestroyMonth(MonthInYearEntry *month)
{
    listMonth.removeOne(month);
    month->deleteLater();
}

void CalendarConfig::DestroyMoon(MoonEntry *moon)
{
    listMoon.removeOne(moon);
    moon->deleteLater();
}

void CalendarConfig::DestroyDay(DayInWeekEntry *day)
{
    listDay.removeOne(day);
    day->deleteLater();
}

void CalendarConfig::on_ConfirmCreation_clicked()
{
    emit savedData();
}

void CalendarConfig::on_Cancel_clicked()
{
    this->close();
}

CalendarConfigData CalendarConfig::toConfigData() const
{
    CalendarConfigData cfg;
    cfg.calendarName = "Custom Calendar";

    cfg.weekLength = listDay.size();
    cfg.dayNames.clear();
    cfg.dayNames.reserve(listDay.size());
    for (const DayInWeekEntry *entry : listDay)
        cfg.dayNames.append(entry->name);

    cfg.months.clear();
    cfg.months.reserve(listMonth.size());
    for (const MonthInYearEntry *entry : listMonth) {
        MonthDefinition md;
        md.name     = entry->name;
        md.dayCount = entry->days;
        cfg.months.append(md);
    }

    cfg.moons.clear();
    cfg.moons.reserve(listMoon.size());
    for (const MoonEntry *entry : listMoon) {
        MoonDefinition moon;
        moon.name            = entry->name;
        moon.cycleLengthDays = static_cast<double>(entry->cycle);
        moon.epochDayOffset  = static_cast<double>(entry->offset);
        cfg.moons.append(moon);
    }

    return cfg;
}
