#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "documenthandler.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// THIS BITCH SHOULD BE ***DESTROYED***. CURRENTLY ITS ONLY FUNCTION IS TO KICKSTART THE FUNCTIONALITY OF THE ENTIRE APPLICATION AFTER THE USER PRESSES A BUTTON.
//  THIS SHOULD BE ON START!!! FOR SOME REASON IF THIS LOGIC IS PUT INTO CONSTRUCTOR AFTER "ui->setupUi(this);" THE ACTION MENU BREAKS (?????)
void MainWindow::on_actionNew_triggered()
{
    dochandl = new DocumentHandler(this);
    QString path = "/home";
    browser = new filebrowser(path, ui->fileBrowser, this);
    connect(browser, &filebrowser::fileSelected, this, &MainWindow::openFromBrowser);
    timer = new QTimer();
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [this]() {
        dochandl->saveFile(ui->textEdit);
    });
}

//SLOTS
// NEW FILE FUNCTIONALITY - UNIMPLEMENTED
void MainWindow::on_actionNew_File_triggered()
{

}
// OPEN FILE FUNCTIONALITY - IMPLEMENTED
void MainWindow::on_actionOpen_File_triggered()
{
    dochandl->loadFile(ui->textEdit, QFileDialog::getOpenFileName(nullptr, tr("Open File"), "/home", tr("Text Files (*.txt);;All Files (*)")));
}
// SAVE FILE FUNCTIONALITY - IMPLEMENTED
void MainWindow::on_actionSave_File_triggered()
{
    dochandl->saveFile(ui->textEdit);
}
// AUTOSAVE FUNCTIONALITY - IMPLEMENTED
void MainWindow::on_textEdit_textChanged()
{
    timer->start(2000);
}
// OPEN FILE - FROM BROWSER, SOON TO BE THE MAIN WAY OF OPENING FILES
void MainWindow::openFromBrowser(const QString &path)
{
    dochandl->loadFile(ui->textEdit, path);
}
