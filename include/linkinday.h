#ifndef LINKINDAY_H
#define LINKINDAY_H

#include <QWidget>
#include <include/noteTypes/calendardata.h>

namespace Ui {
class LinkInDay;
}

class LinkInDay : public QWidget
{
    Q_OBJECT

public:
    explicit LinkInDay(QWidget *parent = nullptr, DayLink *link = nullptr);
    ~LinkInDay();
    DayLink *myLink;

private slots:

    void on_DestroyButton_clicked();

    void on_NoteLinkButton_clicked();

private:
    Ui::LinkInDay *ui;
};

#endif // LINKINDAY_H
