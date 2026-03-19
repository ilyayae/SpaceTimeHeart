#ifndef DAYSLOT_H
#define DAYSLOT_H

#include <QWidget>
#include <QVector>
#include "include/noteTypes/calendardata.h"

namespace Ui {
class DaySlot;
}

class DaySlot : public QWidget
{
    Q_OBJECT

public:
    explicit DaySlot(QWidget *parent = nullptr, int _dayNumber = 0, int _weekday = 0, QVector<DayLink> *_thisDaysLinks = new QVector<DayLink>(), QList<double> *_moonsPhases = new QList<double>);
    ~DaySlot();
    void mousePressEvent(QMouseEvent *event) override;
    int dayNumber;
    int weekday;
    QVector<DayLink> thisDaysLinks;
    QList<double> moonsPhases; // One moons phase is a value between 0 and 1, where 0 is new moon, 0.5 is full and then it goes back to 0 through 0.5- 0.999
    // here is the code to get a singular moon's phase: std::fmod((m_data.config.absoluteDay(year, month, day) - moon.epochDayOffset) / moon.cycleLengthDays, 1.0);

signals:
    void clicked(int day);

private:
    Ui::DaySlot *ui;
};

#endif // DAYSLOT_H
