#ifndef CALENDAREDITOR_H
#define CALENDAREDITOR_H

#include <QMainWindow>
#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QColorDialog>
#include <QListWidget>

#include <include/noteTypes/calendardata.h>
#include <include/dayslot.h>
#include <include/linkinday.h>
#include <include/noteTypes/uuidregistry.h>

namespace Ui {
class CalendarEditor;
}

class CalendarEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit CalendarEditor(QWidget *parent = nullptr, UuidRegistry *registry = nullptr);
    ~CalendarEditor();
    void Initialize(CalendarData data);
    void UpdateCalendar(int year, int month);
    CalendarData myData;
    UuidRegistry *myRegistry;
    int currentYear;
    int currentMonth;
    int selectedDay;

private slots:
    void SelectDay(int day);

    void on_NextMonth_clicked();

    void on_NextYear_clicked();

    void on_PrevYear_clicked();

    void on_PrevMonth_clicked();

    void on_Date_clicked();

    void on_SpecificLink_clicked();

    void on_YearlyLink_clicked();

private:
    Ui::CalendarEditor *ui;
};

#endif // CALENDAREDITOR_H
