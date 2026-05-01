#include "include/calendarconfig.h"
#include "ui_calendarconfig.h"

CalendarConfig::CalendarConfig(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CalendarConfig)
{
    ui->setupUi(this);
    setupEarthStandard();
}

CalendarConfig::~CalendarConfig()
{
    delete ui;
}

void CalendarConfig::setupEarthStandard()
{
    while(!listMonth.isEmpty()) DestroyMonth(listMonth.first());
    while(!listDay.isEmpty())   DestroyDay(listDay.first());
    while(!listMoon.isEmpty())  DestroyMoon(listMoon.first());

    QStringList weekDays = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    for(const QString &dayName : weekDays) {
        on_AddDay_clicked();
        listDay.last()->setMe(dayName);
    }

    struct MonthData { QString name; int days; };
    QList<MonthData> yearMonths = {
        {"January", 31}, {"February", 28}, {"March", 31}, {"April", 30},
        {"May", 31},     {"June", 30},     {"July", 31},  {"August", 31},
        {"September", 30}, {"October", 31}, {"November", 30}, {"December", 31}
    };

    for(const auto &m : yearMonths) {
        on_AddMonth_clicked();
        listMonth.last()->setMe(m.name, m.days);
    }

    on_AddMoon_clicked();
    listMoon.last()->setMe("Moon", 29.53f, 0.0f, "#FFFFFF");
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
    if(listMonth.count() > 1)
    {
        listMonth.removeOne(month);
        month->deleteLater();
    }
}

void CalendarConfig::DestroyMoon(MoonEntry *moon)
{
    if(listMoon.count() > 1)
    {
    listMoon.removeOne(moon);
    moon->deleteLater();
    }
}

void CalendarConfig::DestroyDay(DayInWeekEntry *day)
{
    if(listDay.count() > 1)
    {
    listDay.removeOne(day);
    day->deleteLater();
    }
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
        md.name = entry->name;
        md.dayCount = entry->days;
        cfg.months.append(md);
    }

    cfg.moons.clear();
    cfg.moons.reserve(listMoon.size());
    for (const MoonEntry *entry : listMoon) {
        MoonDefinition moon;
        moon.name = entry->name;
        moon.cycleLengthDays = static_cast<double>(entry->cycle);
        moon.epochDayOffset = static_cast<double>(entry->offset);
        moon.color = entry->colorHex;
        cfg.moons.append(moon);
    }

    return cfg;
}
