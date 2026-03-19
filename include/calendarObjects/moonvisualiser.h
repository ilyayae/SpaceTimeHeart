#ifndef MOONVISUALISER_H
#define MOONVISUALISER_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QColor>

namespace Ui {
class MoonVisualiser;
}

enum MoonPhase
{
    New,
    WaxCrescent,
    Waxing,
    WaxGibbous,
    Full,
    WanGibbous,
    Waning,
    WanCrescent,
    None
};

class MoonVisualiser : public QWidget
{
    Q_OBJECT

public:
    explicit MoonVisualiser(QWidget *parent = nullptr, MoonPhase phase = New, QString color = "");
    ~MoonVisualiser();
    MoonPhase myPhase;
    QString myColor;

private:
    void paintEvent(QPaintEvent *) override;
    Ui::MoonVisualiser *ui;
};

#endif // MOONVISUALISER_H
