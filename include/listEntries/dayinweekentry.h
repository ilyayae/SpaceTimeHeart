#ifndef DAYINWEEKENTRY_H
#define DAYINWEEKENTRY_H

#include <QWidget>

namespace Ui {
class DayInWeekEntry;
}

class DayInWeekEntry : public QWidget
{
    Q_OBJECT

public:
    explicit DayInWeekEntry(QWidget *parent = nullptr);
    ~DayInWeekEntry();
    QString name;
    void setMe(QString Name);

private slots:
    void on_pushButton_clicked();

    void on_LineName_textChanged(const QString &arg1);

signals:
    void destroyMe(DayInWeekEntry *me);

private:
    Ui::DayInWeekEntry *ui;
};

#endif // DAYINWEEKENTRY_H
