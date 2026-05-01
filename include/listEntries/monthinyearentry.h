#ifndef MONTHINYEARENTRY_H
#define MONTHINYEARENTRY_H

#include <QWidget>
#include <QIntValidator>

namespace Ui {
class MonthInYearEntry;
}

class MonthInYearEntry : public QWidget
{
    Q_OBJECT

public:
    explicit MonthInYearEntry(QWidget *parent = nullptr);
    ~MonthInYearEntry();
    QString name;
    int days;
    void setMe(QString Name, int Days);

private slots:
    void on_pushButton_clicked();

    void on_LineDays_textChanged(const QString &arg1);

    void on_LineName_textChanged(const QString &arg1);

signals:
    void destroyMe(MonthInYearEntry *me);

private:
    Ui::MonthInYearEntry *ui;
};

#endif // MONTHINYEARENTRY_H
