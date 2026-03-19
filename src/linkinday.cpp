#include "include/linkinday.h"
#include "ui_linkinday.h"

LinkInDay::LinkInDay(QWidget *parent, DayLink *link)
    : QWidget(parent)
    , ui(new Ui::LinkInDay)
{
    ui->setupUi(this);
    myLink = link;
    ui->Description->setText(myLink->displayLabel);
    ui->frame->setAutoFillBackground(true);
    ui->frame->setStyleSheet("background-color: " + myLink->colorHex + ";");
}


LinkInDay::~LinkInDay()
{
    delete ui;
}

void LinkInDay::on_DestroyButton_clicked()
{

}


void LinkInDay::on_NoteLinkButton_clicked()
{

}

