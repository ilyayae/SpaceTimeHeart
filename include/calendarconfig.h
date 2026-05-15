#ifndef CALENDARCONFIG_H
#define CALENDARCONFIG_H

#include <QMainWindow>
#include "include/listEntries/dayinweekentry.h"
#include "include/listEntries/monthinyearentry.h"
#include "include/listEntries/moonentry.h"
#include "include/listEntries/leapdayentry.h"
#include "include/noteTypes/calendardata.h"
#include "include/listEntries/leapdayexceptionentry.h"

namespace Ui {
class CalendarConfig;
}

class CalendarConfig : public QMainWindow
{
    Q_OBJECT

public:
    explicit CalendarConfig(QWidget *parent = nullptr);
    ~CalendarConfig();
    QList<MonthInYearEntry*> listMonth;
    QList<MoonEntry*> listMoon;
    QList<DayInWeekEntry*> listDay;
    QList<LeapDayEntry*> listLeapDay;
    void DestroyMonth(MonthInYearEntry *month);
    void DestroyMoon(MoonEntry *moon);
    void DestroyDay(DayInWeekEntry *day);
    void DestroyLeapDay(LeapDayEntry *day);
    CalendarConfigData toConfigData() const;
    void setupEarthStandard();
    void updateAllLeapDays();

private slots:
    void on_AddMonth_clicked();
    void on_AddMoon_clicked();
    void on_AddDay_clicked();
    void on_ConfirmCreation_clicked();
    void on_Cancel_clicked();
    void on_AddLeapDay_clicked();

signals:
    void savedData();

private:
    Ui::CalendarConfig *ui;
};

#endif // CALENDARCONFIG_H
