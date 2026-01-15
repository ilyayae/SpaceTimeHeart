#include "configmenu.h"
#include "ui_configmenu.h"
#include <QFileDialog>

ConfigMenu::ConfigMenu(QWidget *parent, QSettings *settings)
    : QWidget(parent)
    , ui(new Ui::ConfigMenu)
{
    ui->setupUi(this);
    setts = settings;
    ui->PTWD_LineEdit->setText(setts->value("general/WorkDirectory", "").toString());
}

ConfigMenu::~ConfigMenu()
{
    delete ui;
}

void ConfigMenu::on_pButton_Save_clicked()
{
    setts->setValue("general/WorkDirectory", ui->PTWD_LineEdit->text());
    setts->setValue("general/FirstLaunch", false);
    setts->sync();
    emit savedSettings();
    //Should the config window close when everyting is saved?
    this->close();
}


void ConfigMenu::on_pButton_Cancel_clicked()
{
    this->close();
}


void ConfigMenu::on_PTWD_BrowseFilesButton_clicked()
{
    ui->PTWD_LineEdit->setText(QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                                 "/home",
                                                                 QFileDialog::ShowDirsOnly
                                                                     | QFileDialog::DontResolveSymlinks));

}

