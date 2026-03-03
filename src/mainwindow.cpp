#include "include/mainwindow.h"
#include "ui_mainwindow.h"
#include "include/documenthandler.h"
#include "include/configmenu.h"

#include <QAbstractItemModel>

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
    else
    {
        initEverything();
    }
}

//SLOTS
// New file functionality
void MainWindow::on_actionNew_File_triggered()
{

}
// Open file functionality
void MainWindow::on_actionOpen_File_triggered()
{
    dochandl->loadFile(QFileDialog::getOpenFileName(nullptr, tr("Open File"), "/home", tr("Text Files (*.txt);;All Files (*)")));
}
// Save file functionality
void MainWindow::on_actionSave_File_triggered()
{
    dochandl->saveFile();
}

// Autosave functionality
void MainWindow::StartSaveProcess()
{
    timer->start(2000);
}

// Opens config menu
void MainWindow::on_actionPreferences_triggered()
{
    configMenu = new ConfigMenu(nullptr, settings);
    connect(configMenu, &ConfigMenu::savedSettings, this, &MainWindow::initEverything);
    configMenu->setWindowTitle("Config Menu");
    configMenu->show();
}

// Function to start the application and init all of the needed objects
void MainWindow::initEverything()
{
    //Instantiate objects
    dochandl = new DocumentHandler(this, settings, ui->EditorPlace);
    browser = new filebrowser(settings->value("general/WorkDirectory", "/home").toString(), ui->fileBrowser, this);

    //Connect signals & slots
    connect(browser, &filebrowser::fileSelected, this, &MainWindow::openFromBrowser);
    connect(dochandl, &DocumentHandler::fileUpdated, this, &MainWindow::StartSaveProcess);
    connect(dochandl, &DocumentHandler::linkFollowed, this, &MainWindow::updateBrowser);

    //Autosave Functionality - Timer settings
    timer = new QTimer();
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [this]() {
        dochandl->saveFile();
    });
}
// Function to open file from filebrowser
void MainWindow::openFromBrowser(const QString &path)
{
    dochandl->loadFile(path);
}

void MainWindow::updateBrowser(const QString &path)
{
    QAbstractItemModel* aim = ui->fileBrowser->model();
    QFileSystemModel* fsm = (QFileSystemModel*)aim;
    QModelIndex index = fsm->index(path);
    if(index.isValid())
    {
        ui->fileBrowser->setCurrentIndex(index);
        ui->fileBrowser->scrollTo(index, QAbstractItemView::PositionAtCenter);
        ui->fileBrowser->expand(index.parent());
    }
}

