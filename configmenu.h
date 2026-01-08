#ifndef CONFIGMENU_H
#define CONFIGMENU_H

#include <QWidget>
#include <QSettings>

namespace Ui {
class ConfigMenu;
}

class ConfigMenu : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigMenu(QWidget *parent = nullptr, QSettings *settings = nullptr);
    ~ConfigMenu();

private slots:
    void on_pButton_Save_clicked();

    void on_pButton_Cancel_clicked();

private:
    Ui::ConfigMenu *ui;
    QSettings *setts;

signals:
    void savedSettings();
};

#endif // CONFIGMENU_H
