#include "include/dayslot.h"
#include "ui_dayslot.h"

DaySlot::DaySlot(QWidget *parent, int _dayNumber, int _weekday, QVector<DayLink> *_thisDaysLinks, QList<double> *_moonsPhases)
    : QWidget(parent)
    , ui(new Ui::DaySlot)
{
    dayNumber = _dayNumber;
    weekday = _weekday;
    thisDaysLinks = *_thisDaysLinks;
    moonsPhases = *_moonsPhases;
    ui->setupUi(this);
    ui->label->setText(QString::number(dayNumber));
}

DaySlot::~DaySlot()
{
    delete ui;
}

void DaySlot::mousePressEvent(QMouseEvent *event)
{
    // Opens a menu at the bottom that has all links for this day and a button to create a new one
    // Each link in the menu is clickable to open the note. There is a button at the very right of the link to delete it from this specific day;
    emit clicked(dayNumber);
    QWidget::mousePressEvent(event);
}
