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
    explicit LinkInDay(QWidget *parent = nullptr, const DayLink &link = DayLink(), QString destinationName = "");
    ~LinkInDay();
    DayLink myLink;

private slots:

    void on_DestroyButton_clicked();

    void on_NoteLinkButton_clicked();

signals:
    void GoToNote(QString link);
    void DestroyLink(DayLink link);

private:
    Ui::LinkInDay *ui;
};

#endif // LINKINDAY_H
