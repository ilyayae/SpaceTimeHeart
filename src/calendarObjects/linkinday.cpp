#include "include/calendarObjects/linkinday.h"
#include "ui_linkinday.h"

LinkInDay::LinkInDay(QWidget *parent, const DayLink &link, QString destinationName)
    : QWidget(parent)
    , ui(new Ui::LinkInDay)
{
    ui->setupUi(this);
    myLink = link;
    ui->Description->setText(myLink.displayLabel);
    ui->frame->setAutoFillBackground(true);
    ui->frame->setStyleSheet("border: 1px solid transparent; border-radius: 10px; background-color: " + myLink.colorHex + ";");
    ui->NoteLinkButton->setText(destinationName);
    ui->NoteLinkButton->setStyleSheet("color: #0066CC; text-decoration: underline; border: none; background: transparent;");
    ui->NoteLinkButton->setCursor(Qt::PointingHandCursor);
}


LinkInDay::~LinkInDay()
{
    delete ui;
}

void LinkInDay::on_DestroyButton_clicked()
{
    emit DestroyLink(myLink);
}


void LinkInDay::on_NoteLinkButton_clicked()
{

    if (myLink.targetNoteId.isEmpty()) {
        qDebug() << "targetNoteId is empty";
        return;
    }
    emit GoToNote(myLink.targetNoteId);
}

