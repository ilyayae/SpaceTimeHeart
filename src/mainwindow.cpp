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


    QTimer::singleShot(0, this, [this]() {
        settings = new QSettings("zhopets", "SpaceTimeHeart");
        if (settings->value("general/FirstLaunch", true).toBool()) {
            on_actionPreferences_triggered();
        } else {
            initEverything();
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;

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
    browser = new filebrowser(settings->value("general/WorkDirectory", "/home").toString(), ui->centralwidget);
    browser->setWindowFlags(Qt::Widget);
    qobject_cast<QHBoxLayout*>(ui->centralwidget->layout())->insertWidget(0, browser);

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
    QAbstractItemModel* aim = browser->view->model();
    QFileSystemModel* fsm = (QFileSystemModel*)aim;
    QModelIndex index = fsm->index(path);
    if(index.isValid())
    {
        browser->view->setCurrentIndex(index);
        browser->view->scrollTo(index, QAbstractItemView::PositionAtCenter);
        browser->view->expand(index.parent());
    }
}

