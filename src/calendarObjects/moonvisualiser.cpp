#include "include/calendarObjects/moonvisualiser.h"
#include "ui_moonvisualiser.h"

MoonVisualiser::MoonVisualiser(QWidget *parent, MoonPhase phase, QString color)
    : QWidget(parent)
    , ui(new Ui::MoonVisualiser)
{
    ui->setupUi(this);
    myPhase = phase;
    myColor = color;
    setFixedSize(16, 16);
}

MoonVisualiser::~MoonVisualiser()
{
    delete ui;
}

void MoonVisualiser::paintEvent(QPaintEvent *)
{

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int size = qMin(width(), height()) - 2;
    int x = (width() - size) / 2;
    int y = (height() - size) / 2;
    QRectF bounds(x, y, size, size);
    double cx = bounds.center().x();
    double cy = bounds.center().y();
    double r  = size / 2.0;

    p.setBrush(myColor);
    p.setPen(Qt::NoPen);
    p.drawEllipse(bounds);
    QPainterPath shadow;
    shadow.addEllipse(bounds);

    QPainterPath lit;
    QColor bg(Qt::black);

    switch(myPhase)
    {
    case New:
    {
        p.setBrush(bg);
        p.drawEllipse(bounds);
        QPen pen(1);
        pen.setColor(QColor::fromString(myColor));
        p.setPen(pen);
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(bounds);
        break;
    }
    case WaxCrescent:
    {
        lit.moveTo(cx, cy - r);
        lit.arcTo(bounds, 90, -180);
        lit.cubicTo(cx + r * 0.6, cy + r * 0.55,
                    cx + r * 0.6, cy - r * 0.55,
                    cx, cy - r);
        QPainterPath sp = shadow.subtracted(lit);
        p.setBrush(bg);
        p.drawPath(sp);
        break;
    }
    case Waxing:
    {
        lit.moveTo(cx, cy - r);
        lit.arcTo(bounds, 90, -180);
        lit.lineTo(cx, cy - r);
        QPainterPath sp = shadow.subtracted(lit);
        p.setBrush(bg);
        p.drawPath(sp);
        break;
    }
    case WaxGibbous:
    {
        lit.moveTo(cx, cy - r);
        lit.arcTo(bounds, 90, -180);
        lit.cubicTo(cx - r * 0.6, cy + r * 0.55,
                    cx - r * 0.6, cy - r * 0.55,
                    cx, cy - r);
        QPainterPath sp = shadow.subtracted(lit);
        p.setBrush(bg);
        p.drawPath(sp);
        break;
    }
    case Full:{
        // NOTHING CHANGES
        break;
    }
    case WanGibbous:
    {
        lit.moveTo(cx, cy - r);
        lit.arcTo(bounds, 90, 180);
        lit.cubicTo(cx + r * 0.6, cy + r * 0.55,
                    cx + r * 0.6, cy - r * 0.55,
                    cx, cy - r);
        QPainterPath sp = shadow.subtracted(lit);
        p.setBrush(bg);
        p.drawPath(sp);
        break;
    }
    case Waning:
    {
        lit.moveTo(cx, cy - r);
        lit.arcTo(bounds, 90, 180);
        lit.lineTo(cx, cy - r);
        QPainterPath sp = shadow.subtracted(lit);
        p.setBrush(bg);
        p.drawPath(sp);
        break;
    }
    case WanCrescent:
    {
        lit.moveTo(cx, cy - r);
        lit.arcTo(bounds, 90, 180);
        lit.cubicTo(cx - r * 0.6, cy + r * 0.55,
                    cx - r * 0.6, cy - r * 0.55,
                    cx, cy - r);
        QPainterPath sp = shadow.subtracted(lit);
        p.setBrush(bg);
        p.drawPath(sp);
        break;
    }
    default:
    {
        p.setBrush(bg);
        p.drawEllipse(bounds);
        break;
    }
    }
}
