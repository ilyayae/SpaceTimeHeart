#ifndef LEAPDAYEXCEPTIONENTRY_H
#define LEAPDAYEXCEPTIONENTRY_H

#include <QWidget>

namespace Ui {
class LeapDayExceptionEntry;
}

class LeapDayExceptionEntry : public QWidget
{
    Q_OBJECT

public:
    explicit LeapDayExceptionEntry(QWidget *parent = nullptr);
    ~LeapDayExceptionEntry();
    bool happens;
    int priority;
    int everyNYears;
    int yearOffset;
    void setMe(bool hap, int prio, int NYears, int offset);

private slots:
    void on_checkBoxHappens_checkStateChanged(const Qt::CheckState &arg1);

    void on_spinBoxPriority_valueChanged(int arg1);

    void on_spinBoxYearsOffset_valueChanged(int arg1);

    void on_spinBoxEveryNYears_valueChanged(int arg1);

    void on_RemoveButton_clicked();

signals:
    void destroyMe(LeapDayExceptionEntry *me);


private:
    Ui::LeapDayExceptionEntry *ui;
};

#endif // LEAPDAYEXCEPTIONENTRY_H
