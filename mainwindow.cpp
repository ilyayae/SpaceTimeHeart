#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "documenthandler.h"
#include "configmenu.h"


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
    //First launch settings

    settings = new QSettings("zhopets", "SpaceTimeHeart");
    if (settings->value("general/FirstLaunch", true).toBool())
    {
        MainWindow::on_actionPreferences_triggered();
    }
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
// Opens config menu
void MainWindow::on_actionPreferences_triggered()
{
    //qDebug() << "I work" ;
    c = new ConfigMenu(nullptr, settings);
    connect(c, &ConfigMenu::savedSettings, this, &MainWindow::initEverything);
    c->setWindowTitle("Config Menu");
    c->show();
}

// Function to start the application and init all of the shit needed
void MainWindow::initEverything()
{
    //Instantiate objects
    dochandl = new DocumentHandler(this, settings);
    browser = new filebrowser(settings->value("general/WorkDirectory", "/home").toString(), ui->fileBrowser, this);


    //Connect signals & sockets
    connect(browser, &filebrowser::fileSelected, this, &MainWindow::openFromBrowser);

    //Autosave Functionality - Timer settings
    timer = new QTimer();
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [this]() {
        dochandl->saveFile(ui->textEdit);
    });
    qDebug() << "I twerk" ;
}
// Function to open file from filebrowser
void MainWindow::openFromBrowser(const QString &path)
{
    dochandl->loadFile(ui->textEdit, path);
}

