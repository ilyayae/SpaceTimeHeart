#include "include/calendarObjects/dayslot.h"
#include "ui_dayslot.h"

DaySlot::DaySlot(QWidget *parent, int _dayNumber, int _weekday, QVector<DayLink> *_thisDaysLinks, QList<double> *_moonsPhases, QList<QString> *_moonsColors)
    : QWidget(parent)
    , ui(new Ui::DaySlot)
{
    dayNumber = _dayNumber;
    weekday = _weekday; // unused
    thisDaysLinks = *_thisDaysLinks;
    moonsPhases = *_moonsPhases;
    moonsColors = *_moonsColors;
    ui->setupUi(this);
    UpdateEventTracker();
    ui->label->setText(QString::number(dayNumber));
    for(int i = 0; i < moonsPhases.count(); i++)
    {
        // convert moonsPhases[i] into a moon phase
        MoonPhase mp;
        if(moonsPhases[i] >= 0.94 || moonsPhases[i] <= 0.06) mp = MoonPhase::New;
        else if(moonsPhases[i] >= 0.06 && moonsPhases[i] <= 0.19) mp = MoonPhase::WaxCrescent;
        else if(moonsPhases[i] >= 0.19 && moonsPhases[i] <= 0.31) mp = MoonPhase::Waxing;
        else if(moonsPhases[i] >= 0.31 && moonsPhases[i] <= 0.44) mp = MoonPhase::WaxGibbous;
        else if(moonsPhases[i] >= 0.44 && moonsPhases[i] <= 0.56) mp = MoonPhase::Full;
        else if(moonsPhases[i] >= 0.56 && moonsPhases[i] <= 0.69) mp = MoonPhase::WanGibbous;
        else if(moonsPhases[i] >= 0.69 && moonsPhases[i] <= 0.81) mp = MoonPhase::Waning;
        else if(moonsPhases[i] >= 0.81 && moonsPhases[i] <= 0.94) mp = MoonPhase::WanCrescent;
        else mp = MoonPhase::None;
        if (mp == MoonPhase::None)
            continue;
        MoonVisualiser *mv = new MoonVisualiser(ui->Moons, mp, moonsColors[i]);
        ui->Moons->layout()->addWidget(mv);
    }
    ui->Moons->layout()->setAlignment(Qt::AlignLeft);
}

DaySlot::~DaySlot()
{
    delete ui;
}

void DaySlot::mousePressEvent(QMouseEvent *event)
{
    // Opens a menu at the bottom that has all links for this day and a button to create a new one
    // Each link in the menu is clickable to open the note. There is a button at the very right of the link to delete it from this specific day;
    emit clicked(this);

    QWidget::mousePressEvent(event);
}

void DaySlot::ColorMe(QString color)
{
    ui->HolderOfAll->setStyleSheet("border: 1px solid black; border-radius: 10px; background: " + color + ";");
}

void DaySlot::UpdateEventTracker()
{
    if(thisDaysLinks.count() != 0)
    {
        ui->eventsLabel->setText("(" + QString::number(thisDaysLinks.count()) + ")");
        ui->eventsLabel->setStyleSheet("color: black; border-radius: 5px; background: #ffc800;");
    }
}
