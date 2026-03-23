#ifndef DAYSLOT_H
#define DAYSLOT_H

#include <QWidget>
#include <QVector>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include "include/noteTypes/calendardata.h"
#include <include/calendarObjects/moonvisualiser.h>

namespace Ui {
class DaySlot;
}

class DaySlot : public QWidget
{
    Q_OBJECT

public:
    explicit DaySlot(QWidget *parent = nullptr, int _dayNumber = 0, int _weekday = 0, int _thisDaysLinks = 0, QList<double> *_moonsPhases = new QList<double>, QList<QString> *_moonsColors = new QList<QString>);
    ~DaySlot();
    void mousePressEvent(QMouseEvent *event) override;
    void UpdateEventTracker();
    int dayNumber;
    int weekday;
    int thisDaysLinks;
    QList<double> moonsPhases; // One moons phase is a value between 0 and 1, where 0 is new moon, 0.5 is full and then it goes back to 0 through 0.5- 0.999
    QList<QString> moonsColors;
    void ColorMe(QString color);

signals:
    void clicked(DaySlot *me);

private:
    Ui::DaySlot *ui;
};

#endif // DAYSLOT_H
