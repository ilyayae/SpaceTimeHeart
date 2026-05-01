#ifndef MOONENTRY_H
#define MOONENTRY_H

#include <QWidget>
#include <QDoubleValidator>
#include <QColorDialog>

namespace Ui {
class MoonEntry;
}

class MoonEntry : public QWidget
{
    Q_OBJECT

public:
    explicit MoonEntry(QWidget *parent = nullptr);
    ~MoonEntry();
    QString name;
    float cycle;
    float offset;
    QString colorHex;
    void setMe(QString Name, float Cycle, float Offset, QString Color);

private slots:
    void on_RemoveButton_clicked();

    void on_LineName_textChanged(const QString &arg1);

    void on_LineCycle_textChanged(const QString &arg1);

    void on_LineOffset_textChanged(const QString &arg1);

    void on_ColorButton_clicked();

signals:
    void destroyMe(MoonEntry *me);


private:
    Ui::MoonEntry *ui;
};

#endif // MOONENTRY_H
