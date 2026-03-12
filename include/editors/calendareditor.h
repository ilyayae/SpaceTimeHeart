#ifndef CALENDAREDITOR_H
#define CALENDAREDITOR_H

#include <QMainWindow>

namespace Ui {
class CalendarEditor;
}

class CalendarEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit CalendarEditor(QWidget *parent = nullptr);
    ~CalendarEditor();

private:
    Ui::CalendarEditor *ui;
};

#endif // CALENDAREDITOR_H
