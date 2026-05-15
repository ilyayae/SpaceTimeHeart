#ifndef LEAPDAYENTRY_H
#define LEAPDAYENTRY_H

#include "include/listEntries/monthinyearentry.h"
#include "include/listEntries/leapdayexceptionentry.h"
#include "include/noteTypes/calendardata.h"
#include <QWidget>

namespace Ui {
class LeapDayEntry;
}

class LeapDayEntry : public QWidget
{
    Q_OBJECT
public:
    explicit LeapDayEntry(QWidget *parent = nullptr);
    ~LeapDayEntry();
    void setMonthSelection(QList<MonthInYearEntry*> listOfMonths);
    int selectedMonth = 2;
    int everyNYears = 4;
    int offsetYears = 0;
    int daysAdded = 1;
    bool affectsWeekDays = true;
    bool randomizeDay = false;
    bool randomizeMonth = false;
    QList<LeapDayExceptionEntry*> exceptions;
    void DestroyLeap(LeapDayExceptionEntry *leap);
    void setMe(int month = 1, int NYears = 4, int offset = 0, int addDays = 1, bool affectWeekDays = true, bool randomDay = false, bool randomMonth = false, QList<LeapDayException> exception = QList<LeapDayException>());

private:
    Ui::LeapDayEntry *ui;

signals:
    void destroyMe(LeapDayEntry *me);

private slots:
    void on_MonthSelection_activated(int index);
    void on_EveryNYear_valueChanged(int arg1);
    void on_Offset_valueChanged(int arg1);
    void on_DaysAdded_valueChanged(int arg1);
    void on_AffectsWeekDay_checkStateChanged(const Qt::CheckState &arg1);
    void on_RandomizeDay_checkStateChanged(const Qt::CheckState &arg1);
    void on_RandomizeMonth_checkStateChanged(const Qt::CheckState &arg1);
    void on_AddException_clicked();
    void on_DeleteLeapDay_clicked();
};

#endif // LEAPDAYENTRY_H
